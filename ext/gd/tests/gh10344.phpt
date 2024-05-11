--TEST--
GH-10344 (imagettfbbox(): Could not find/open font UNC path)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (PHP_OS_FAMILY != 'Windows') {
    die("skip windows only test");
}
if (!GD_BUNDLED) {
    die("skip Not (yet) fixed in libgd itself");
}
if (!function_exists("imagettfbbox")) {
    die("skip ttf support unavailable");
}
if (!is_readable("\\\\localhost\\c$\\Windows\\Fonts\\arial.ttf")) {
    die("skip font not readable");
}
?>
--FILE--
<?php

$font = '\\\\localhost\\c$\\Windows\\Fonts\\arial.ttf';
var_dump(count(imagettfbbox(10, 0, $font, 'hello')));

?>
--EXPECT--
int(8)
