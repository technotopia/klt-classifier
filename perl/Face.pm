package Face;
use File::Slurp qw(write_file);
use Storable qw(dclone);
use POSIX;

my $size = 21;

sub columnsQuantity {
    return $size * 2; #only x and y
}

sub normal {
    my ($sigma, $estimate) = @_;
    $sigma //= 1;
    $estimate //= 0;
    
    my $val = 0;
    for (1 .. 9) {
        $val += $sigma/3 * (2 * rand() - 1) ;
    }
    return $val;
}

sub randomFace {
    my $class = shift;
    my $pid = shift;
    my $params = {
        pid => $pid,
        nose => {length => normal(2), height => normal(2), width => normal(2)}, 
        eyes => {depth => normal(1), height => normal(2), width => normal(2)}, 
        scullBones => {depth => normal(1), width => normal(2), height => normal(2), tilt => normal(1)}, 
        chin => {width => normal(1), height => normal(1), shape => normal(1)}
    };
    return $class->new($params);    
}

sub new {
    my ($class, $params) = @_;
    die ("bad face\n") unless (defined($params) && defined($params->{pid}));
    
    my $points = [];
    $points->[0] = [100, 120, 0];
    $points->[1] = [80, 120, 0];
    $points->[2] = [120, 90, 0];
    $points->[3] = [60, 90, 0];
    $points->[4] = [100, 60, 0];
    $points->[5] = [80, 60, 0];
    
    #eyes
    my $eyes = $params->{eyes} // {};
    $eyes->{width} //= 0;
    $eyes->{height} //= 0;
    $eyes->{depth} //= 0;
    $eyes->{tilt} //= 0;
    
    
    $points->[6] = [100 + $eyes->{width}, 105 + $eyes->{height}, 5 + $eyes->{depth}]; 
    $points->[7] = [80 - $eyes->{width}, 105 + $eyes->{height}, 5 + $eyes->{depth}];
    $points->[8] = [104 + $eyes->{width}, 105 + $eyes->{height} + $eyes->{tilt}, 4 + $eyes->{depth}]; 
    $points->[9] = [76 - $eyes->{width}, 105 + $eyes->{height} + $eyes->{tilt}, 4 + $eyes->{depth}];
    
    #nose
    my $nose = $params->{nose} // {};
    $nose->{width} //= 0;
    $nose->{height} //= 0;
    $nose->{length} //= 0;
    
    $points->[10] = [90, 98 + $nose->{height}, 2]; 
    $points->[11] = [87 - $nose->{width}, 90 + $nose->{height}, 0];
    $points->[12] = [93 + $nose->{width}, 90 + $nose->{height}, 0];
    $points->[13] = [93 + $nose->{width}, 90 + $nose->{height} - $nose->{length}, -5 - $nose->{length}];
    
    my $scullBones = $params->{scullBones} // {};
    $scullBones->{width} //= 0;
    $scullBones->{height} //= 0;
    $scullBones->{depth} //= 0;
    $scullBones->{tilt} //= 0;
    
    $points->[14] = [99 - $scullBones->{width}, 95 + $scullBones->{height}, -2 + $scullBones->{depth}];
    $points->[15] = [81 + $scullBones->{width}, 95 + $scullBones->{height}, -2 + $scullBones->{depth}];
    $points->[16] = [102 - $scullBones->{width}, 96 + $scullBones->{height} + $scullBones->{tilt}, $scullBones->{depth} + $scullBones->{tilt}];
    $points->[17] = [78 + $scullBones->{width}, 96 + $scullBones->{height} + $scullBones->{tilt}, $scullBones->{depth} + $scullBones->{tilt}];
    
    
    my $chin = $params->{chin} // {};
    $chin->{width} //= 0;
    $chin->{height} //= 0;
    $chin->{shape} //= 0;
    
    $points->[18] = [83 - $chin->{width}, 55 + $chin->{height} - $chin->{shape}, 1 + $chin->{depth}];
    $points->[19] = [97 + $chin->{width}, 55 + $chin->{height} - $chin->{shape}, 1 + $chin->{depth}];
    $points->[20] = [90, 53 + $chin->{height} + $chin->{shape}, 1 + $chin->{depth} + $chin->{shape}];
    
    my $face = {
        points => $points,
        params => $params
    };
    return bless($face, $class);
}

sub copy {
    my $self = shift;
    return dclone($self);
}

sub print {
    my $self = shift;
    $points = $self->{points};
    my $string = "";
    $string .= $self->{params}->{pid};
    foreach my $point (@{$self->{points}}) {
        $string .= "," . POSIX::floor($point->[0]);
    }
    foreach my $point (@{$self->{points}}) {
        $string .= "," . POSIX::floor($point->[1]);
    }
    return $string;
}

sub move {
    my ($face, $mx, $my, $mz) = @_;    
    my $new_points = [];
    foreach my $point (@{$face->{points}}) {
        my ($x,$y, $z) = @$point;
        my $point = [
            ($mx + $x),
            ($my + $y),
            ($mz + $z),
        ];
        push(@$new_points, $point);
    }
    $face->{points} = $new_points;
    return $face;
}

sub scale {
    my ($face, $sx, $sy, $sz) = @_;    
    my ($cx, $cy, $cz) = (100, 100, 0); 
    
    my $new_points = [];
    foreach my $point (@{$face->{points}}) {
        my ($x,$y) = @$point;
        my $point = [
            ($cx + $sx * ($x - $cx)),
            ($cy + $sy * ($y - $cy)),
            ($cz + $sz * ($z - $cz))
        ];
        push(@$new_points, $point);
    }
    $face->{points} = $new_points;
    return $face;
}

sub noise {
    my ($face, $noise) = @_;
    my $new_points = [];
    foreach my $point (@{$face->{points}}) {
        my ($x,$y) = @$point;
        my $point = [
            $x += $noise * (2 * rand() - 1),
            $y += $noise * (2 * rand() - 1),
        ];
        push(@$new_points, $point);
    }
    $face->{points} = $new_points;
    return $face;
}

sub rotate {
    my ($face, $phiz, $phiy, $phix) = @_;
    my ($cx, $cy, $cz) = (100, 100, 0); 
    my $new_points = [];
    my $cosz = cos($phiz);
    my $sinz = sin($phiz);
    
    my $cosy = cos($phiy);
    my $siny = sin($phiy);
    
    my $cosx = cos($phix);
    my $sinx = sin($phix);
    
    foreach my $point (@{$face->{points}}) {
        my ($x, $y, $z) = @$point;
        $x = $x - $cx;
        $y = $y - $cy;
        $z = $z - $cz;
        
        $x = $x * $cosz + $y * $sinz;
        $y = - $x * $sinz + $y * $cosz;
    
        $y = $y * $cosx + $z * $sinx;
        $z = - $y * $sinx + $z * $cosx;

        $z = $z * $cosy + $x * $siny;
        $x = - $z * $siny + $x * $cosy;

        my $point = [
            $cx + $x,
            $cy + $y,
            $cz + $z
        ];
        push(@$new_points, $point);
    }
    $face->{points} = $new_points;
    return $face;
}

return 1;