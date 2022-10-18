--TEST--
Bug #81739 (OOB read due to insufficient validation in imageloadfont())
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip gd extension not available");
?>
--FILE--
<?php
$s = fopen(__DIR__ . "/font.font", "w");
// header without character data
fwrite($s, "\x01\x00\x00\x00\x20\x00\x00\x00\x08\x00\x00\x00\x08\x00\x00\x00");
fclose($s);
var_dump(imageloadfont(__DIR__ . "/font.font"));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/font.font");
?>
--EXPECTF--
Warning: imageloadfont(): %croduct of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d

Warning: imageloadfont(): Error reading font, invalid font header in %s on line %d
bool(false)