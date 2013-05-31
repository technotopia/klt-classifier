use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/.";
use Face;



use File::Slurp qw(read_file write_file);
use Data::Dumper;
use POSIX;

my $file = (shift or "sample");
my $noiseLevel = 0;
my $sampleNoise = 0;
my $combatNoiseLevel = 1;
my $rotationLevel = 0.4;
my $blur = 1.2;

sub print_faces {
    my ($faces, $file) = @_;
    my $text = Face::columnsQuantity() . "\n";
    while (my ($id, $face) = each (%$faces)) {
        $text .= "$id,";
        $text .= $face->print;
        $text .= "\n";
    }
    write_file($file, $text);
}


my $pid = 1;
my $id = 1;

sub add_rotated {
    my ($faces, $face, $blur, $times) = @_;
    $blur //= 1;
    for (1 .. $times) {
        srand();
        my $scale = rand() + 0.5;
        
        my $phiz = rand() - 0.5;
        $phiz *= $blur;
        my $phix = rand() - 0.5;
        $phix *= $blur;
        my $phiy = rand() - 0.5;
        $phiy *= $blur;
        my $movex = rand(100) - 50;
        my $movey = rand(100) - 50;
        $faces->{$id++} = $face->copy()->scale($scale,$scale,$scale)->rotate($phiz, $phiy, $phix)->move($movex, $movey, 0)->noise($noiseLevel);
    }    
}




my $faces = {};
my $learningQuantity = 100;
my $face = Face->new({pid => $pid++, eyes => {depth => -3, width => 2, height => -1, tilt => 1}, nose => {length => 2, width => -1, height => 2}, scullBones => {depth => 1, width => 2, height => -1, tilt => 1}, chin => {width => 2, height => -1, shape => 1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => 3, width => 3, height => 1}, nose => {length => -1, width => 3, height => -2}, scullBones => {depth => -1, width => 0, height => 1, tilt => -1}, chin => {width => 1, height => 1, shape => 0}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => -1, width => 4, height => -2, tilt => -1}, nose => {length => 1, width => 1, height => 1}, scullBones => {depth => 0, width => 2, height => 0}, chin => {width => -1, height => -2, shape => -1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => 0, width => 2, height => -1}, nose => {length => 3, width => 2, height => 2}, scullBones => {depth => 0, width => -1, height => -1, tilt => -1}, chin => {width => 1, height => 0, shape => 2}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => 1, width => 1, height => -1, tilt => 1}, nose => {length => 0, width => -1, height => 0}, scullBones => {depth => -1, width => 0, height => 0}, chin => {width => 0, height => 1, shape => -1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => -2, width => 2, height => 0, tilt => -1}, nose => {length => -2, width => 3, height => 4}, scullBones => {depth => -1, width => 1, height => 1, tilt => -1}, chin => {width => 1, height => 1, shape => 2}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => 2, width => -2, height => 2, tilt => -1}, nose => {length => 2, width => -2, height => -2}, scullBones => {depth => -1, width => 1, height => -1}, chin => {width => 1, height => 1, shape => -1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => -1, width => -2, height => -2}, nose => {length => 2, width => 2, height => 2}, scullBones => {depth => 1, width => 1, height => 1}, chin => {width => -1, height => 2, shape => -1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => 3, width => -2, height => -2, tilt => 1}, nose => {length => 2, width => 2, height => 2}, scullBones => {depth => -2, width => 1, height => -1, tilt => -1}, chin => {width => 0, height => 1, shape => -2}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => -3, width => -3, height => 2, tilt => -1}, nose => {length => -2, width => -2, height => -2}, scullBones => {depth => -1, width => 1, height => 1, tilt => 1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => -1, width => 1, height => -2, tilt => 0}, nose => {length => 0, width => 1, height => -1}, scullBones => {depth => 2, width => -1, height => -2, tilt => 1}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => 1, width => 2, height => 0, tilt => -1}, nose => {length => 2, width => 0, height => 1}, scullBones => {depth => -1, width => 2, height => 2, tilt => -1}, chin => {width => 1, height => 1, shape => 2}});
add_rotated($faces, $face, $blur, $learningQuantity);

$face = Face->new({pid => $pid++, eyes => {depth => -2, width => -1, height => 1, tilt => 1}, nose => {length => 1, width => 1, height => -1}, scullBones => {depth => 2, width => 3, height => -1, tilt => 0}, chin => {width => -1, height => -1, shape => 0}});
add_rotated($faces, $face, $blur, $learningQuantity);

for (1 .. 100) {
    $face = Face->randomFace($pid++);
    add_rotated($faces, $face, $blur, $learningQuantity);
}





print_faces($faces, "learning");

my $samples = {};
my $tests = {};

$noiseLevel = $combatNoiseLevel;

#0
my $testQuantity = 2; # number of test faces per each sample
$id = 0;
$face = Face->new({pid => $pid++, eyes => {depth => -2, height => 1, width => 1}, scullBones => {depth => 2, width => -1, height => -2, tilt => 1}, chin => {width => -1, height => -1, shape => 0}});
add_rotated($tests, $face, $blur, $testQuantity); 
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#1
$face = Face->new({pid => $pid++, nose => {length => 2, height => -2, width => -1}, scullBones => {depth => -2, width => 1, height => 0}, chin => {width => 2, height => 2, shape => 2}});
add_rotated($tests, $face, $blur, $testQuantity); 
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#2
$face = Face->new({pid => $pid++, eyes => {depth => 1, height => -1, width => 1}, scullBones => {depth => -2, width => 1, height => 0}, chin => {width => -1, height => 0, shape => -1}});
add_rotated($tests, $face, $blur, $testQuantity); 
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#3
$face = Face->new({pid => $pid++, nose => {length => 0, height => 2, width => 2}, eyes => {depth => -2, height => 0, width => -1}, scullBones => {depth => -1, width => 1, height => -2, tilt => 1}, chin => {width => 1, height => 1, shape => 0}});
add_rotated($tests, $face, $blur, $testQuantity);
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#4
$face = Face->new({pid => $pid++, nose => {length => -2, height => 0, width => -1}, eyes => {depth => 0, height => 2, width => -2}, scullBones => {depth => -1, width => 1, height => -2, tilt => 0}, chin => {width => 0, height => 0, shape => 2}});
add_rotated($tests, $face, $blur, $testQuantity);
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#5
$face = Face->new({pid => $pid++, nose => {length => 1, height => 3, width => 0}, eyes => {depth => 2, height => 0, width => -1}, scullBones => {depth => -1, width => 3, height => 1, tilt => -1}, chin => {width => 1, height => 1, shape => 0}});
add_rotated($tests, $face, $blur, $testQuantity);
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#6
$face = Face->new({pid => $pid++, nose => {length => 1, height => 0, width => 0}, eyes => {depth => 4, height => 0, width => -1}, scullBones => {depth => 0, width => 2, height => -2}, chin => {width => -2, height => 0, shape => 1}});
add_rotated($tests, $face, $blur, $testQuantity);
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#7
$face = Face->new({pid => $pid++, nose => {length => 0, height => 3, width => 1}, eyes => {depth => 1, height => 1, width => 1}, scullBones => {depth => 2, width => 1, height => 2, tilt => -1}, chin => {width => -1, height => 0, shape => -1}});
add_rotated($tests, $face, $blur, $testQuantity);
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

#8
$face = Face->new({pid => $pid++, nose => {length => -1, height => -1, width => -1}, eyes => {depth => 2, height => 1, width => -1}, scullBones => {depth => 1, width => 1, height => -1, tilt => 0}, chin => {width => -2, height => 1, shape => -2}});
add_rotated($tests, $face, $blur, $testQuantity);
$samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);

for (1 .. 5000) {
    $face = Face->randomFace($pid++);
    add_rotated($tests, $face, $blur, $testQuantity);
    $samples->{$id++} = $face->copy()->rotate($rotationLevel, $rotationLevel)->noise($sampleNoise);
}

print_faces($samples, "samples");
print_faces($tests, "tests");