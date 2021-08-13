--TEST--
Bug #60160 (imagefill does not work correctly for small images) @see bug51671
--EXTENSIONS--
gd
--FILE--
<?php
$w = 3;
$h = 50;
$im = imagecreatetruecolor($w, $h);
$white = imagecolorallocate($im, 255, 255, 255);
imagefill($im, 0, 0, $white);

for ($ix = 0; $ix < $w; $ix++) {
        for ($iy = 0; $iy < $h; $iy++) {
                if (($c = imagecolorat($im, $ix, $iy)) != $white) {
                        printf("Failed, ($ix, $iy) is %X\n", $c);
                }
        }
}

echo "OK\n";
?>
--EXPECT--
OK
