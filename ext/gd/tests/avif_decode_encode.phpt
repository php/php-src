--TEST--
avif decoding/encoding tests
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecreatefrompng") || !function_exists("imagepng")) {
        die("skip png support unavailable");
    }
    if (!function_exists("imagecreatefromavif") || !function_exists("imageavif")) {
        die("skip avif support unavailable");
    }
?>
--FILE--
<?php

    require_once __DIR__ . '/similarity.inc';

    $infile = __DIR__  . '/girl.avif';
    $outfile = __DIR__  . '/test.avif';

    echo 'Decoding AVIF image: ';
    $img = imagecreatefromavif($infile);
    echo_status($img);

    echo 'Default AVIF encoding: ';
    echo_status(imageavif($img, $outfile));

    echo 'Encoding AVIF at quality 70: ';
    echo_status(imageavif($img, $outfile, 70));

    echo 'Encoding AVIF at quality 70 with speed 5: ';
    echo_status(imageavif($img, $outfile, 70, 5));

    echo 'Encoding AVIF with default quality: ';
    echo_status(imageavif($img, $outfile, -1));

    echo 'Encoding AVIF with illegal quality: ';
    echo_status(imageavif($img, $outfile, 1234));

    echo 'Encoding AVIF with illegal speed: ';
    echo_status(imageavif($img, $outfile, 70, 1234));

    echo 'Encoding AVIF losslessly... ';
    echo_status(imageavif($img, $outfile, 100, 0));

    echo "Decoding the AVIF we just wrote...\n";
    $img_from_avif = imagecreatefromavif($outfile);

    // Note we could also forgive a certain number of pixel differences.
    // With the current test image, we just didn't need to.
    echo 'How many pixels are different in the two images? ';
    print_r(calc_image_dissimilarity($img, $img_from_avif));

    unlink($outfile);


    function echo_status($success) {
        echo $success ? "ok\n" : "failed\n";
    }
?>

--EXPECT--
Decoding AVIF image: ok
Default AVIF encoding: ok
Encoding AVIF at quality 70: ok
Encoding AVIF at quality 70 with speed 5: ok
Encoding AVIF with default quality: ok
Encoding AVIF with illegal quality: ok
Encoding AVIF with illegal speed: ok
Encoding AVIF losslessly... ok
Decoding the AVIF we just wrote...
How many pixels are different in the two images? 0
