--TEST--
GH-19739 (integer overflow in imageellipse / imagefilledellipse)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
$im = imagecreatetruecolor(400, 300);
$color = imagecolorallocate($im, 150, 255, 0);

var_dump(imageellipse($im, 64, 150, 2147483647, 2147483647, $color));
var_dump(imagefilledellipse($im, 64, 150, 2147483647, 2147483647, $color));

echo "done" . PHP_EOL;
?>
--EXPECT--
bool(true)
bool(true)
done
