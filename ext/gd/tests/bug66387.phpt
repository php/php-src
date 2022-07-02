--TEST--
Bug #66387 (Stack overflow with imagefilltoborder)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.1', '<')) {
    die("skip test requires GD 2.2.1 or higher");
}
?>
--FILE--
<?php
$im = imagecreatetruecolor(20, 20);
$c = imagecolorallocate($im, 255, 0, 0);
imagefilltoborder($im, 0, -999355, $c, $c);
echo "ready\n";
?>
--EXPECT--
ready
