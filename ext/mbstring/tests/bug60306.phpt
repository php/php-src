--TEST--
Bug #60306 (Characters lost while converting from cp936 to utf8)
--EXTENSIONS--
mbstring
--FILE--
<?php
$s = "洪仁玕";
var_dump($s === mb_convert_encoding(mb_convert_encoding($s, "cp936", "utf8"), "utf8", "cp936"));
?>
--EXPECT--
bool(true)
