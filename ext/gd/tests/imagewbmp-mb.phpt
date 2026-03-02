--TEST--
imagewbmp
--EXTENSIONS--
gd
--FILE--
<?php
$file = __DIR__ . '/im私はガラスを食べられます.wbmp';

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imagesetpixel($im, 3,3, 0x0);
imagewbmp($im, $file);

$im2 = imagecreatefromwbmp($file);
echo 'test create wbmp: ';
$c = imagecolorsforindex($im2, imagecolorat($im2, 3,3));
$failed = false;
foreach ($c as $v) {
    if ($v != 0) {
        $failed = true;
    }
}
echo !$failed ? 'ok' : 'failed';
echo "\n";
unlink($file);
?>
--EXPECT--
test create wbmp: ok
