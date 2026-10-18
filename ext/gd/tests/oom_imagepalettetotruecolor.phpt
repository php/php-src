--TEST--
imagepalettetotruecolor() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$im = imagecreate(6000, 6000);      // palette image, one byte per pixel
imagepalettetotruecolor($im);       // needs four bytes per pixel while the source stays live
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
