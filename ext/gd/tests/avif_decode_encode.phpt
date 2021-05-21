--TEST--
avif decoding/encoding tests
--SKIPIF--
<?php
    if (!extension_loaded('gd')) {
        die("skip gd extension not available.");
    }

    if (!function_exists("imagecreatefrompng") || !function_exists("imagepng")) {
        die("skip png support unavailable");
    }
    if (!function_exists("imagecreatefromavif") || !function_exists("imageavif")) {
        die("skip avif support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;
    $infile = $cwd . '/girl.avif';
    $outfile = $cwd . '/test.avif';

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
