--TEST--
GH-20622 (imagestring/imagestringup overflow/underflow)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreate(64, 64);
imagestringup($im, 5, 0, -2147483648, 'STRINGUP', 0);
imagestring($im, 5, -2147483648, 0, 'STRING', 0);
echo "OK";
?>
--EXPECT--
OK
