--TEST--
imageloadfont() "End of file while reading header"
--CREDITS--
Ike Devolder <ike.devolder@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php
if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$filename = dirname(__FILE__) .  '/font.gdf';
$bin = "\x41\x41\x41\x41\x00\x00\x00\x00\x00\x00";
$fp = fopen($filename, 'wb');
fwrite($fp, $bin);
fclose($fp);

$font = imageloadfont($filename);
?>
--CLEAN--
<?php
unlink(__DIR__.'/font.gdf');
?>
--EXPECTF--
Warning: imageloadfont(): End of file while reading header in %s on line %d
