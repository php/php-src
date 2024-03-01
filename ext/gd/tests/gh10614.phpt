--TEST--
GH-10614 (imagerotate will turn the picture all black, when rotated 90)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.4', '>=')) die("skip test requires GD 2.3.4 or older");
?>
--FILE--
<?php

$input = imagecreatefrompng(__DIR__ . '/gh10614.png');

for ($angle = 0; $angle <= 270; $angle += 90) {
    echo "--- Angle $angle ---\n";
    $output = imagerotate($input, $angle, 0);
    for ($i = 0; $i < 4; $i++) {
        for ($j = 0; $j < 4; $j++) {
            var_dump(dechex(imagecolorat($output, $i, $j)));
        }
    }
}

?>
--EXPECT--
--- Angle 0 ---
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
--- Angle 90 ---
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
--- Angle 180 ---
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
--- Angle 270 ---
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
string(1) "0"
string(1) "0"
string(8) "7f000000"
string(8) "7f000000"
