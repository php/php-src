--TEST--
Bug #51263 (imagettftext and rotated text uses wrong baseline)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if(!function_exists('imagettftext')) die('skip imagettftext() not available');
?>
--FILE--
<?php
$cwd = __DIR__;
$ttf = "$cwd/Tuffy.ttf";
$w = 23;
$h = 70;
$img = imagecreatetruecolor($w, $h);
$blk = imagecolorallocate($img, 0, 0, 0);
imagefilledrectangle($img, 0, 0, $w-1, $h-1, $blk);
$col = imagecolorallocate($img, 255, 255, 255);
imagettftext($img, 8, 90, 10, 60, $col, $ttf, "foo bar qux");
$x = array(0, 1, 2, 3, 13);
for ($j=0; $j<30; $j++) {
    foreach ($x as $i) {
        $c = imagecolorat($img, $i, $j);
        if ($c != 0) {
            echo "KO: ($i, $j)\n";
            exit;
        }
    }
}
echo "OK\n";
?>
--EXPECT--
OK
