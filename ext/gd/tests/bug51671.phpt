--TEST--
Bug #51671 (imagefill does not work correctly for small images)
--EXTENSIONS--
gd
--FILE--
<?php
$w = 1;
$h = 50;
$im = imagecreatetruecolor($w, $h);
$white = imagecolorallocate($im, 255, 255, 255);
imagefill($im, 0, 0, $white);

for ($iy = 0; $iy < $h; $iy++) {
        if (($c = imagecolorat($im, 0, $iy)) != $white) {
                printf("Failed, (0, $iy) is %X\n", $c);
        }
}

echo "OK\n";
?>
--EXPECT--
OK
