--TEST--
Bug #62545 (wrong unicode mapping in some charsets)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php
var_dump(
    bin2hex(mb_convert_encoding("\x98", 'UTF-8', 'Windows-1251')),
    bin2hex(mb_convert_encoding("\x81\x8d\x8f\x90\x9d", 'UTF-8', 'Windows-1252'))
);
?>
===DONE===
--EXPECT--
string(6) "efbfbd"
string(30) "efbfbdefbfbdefbfbdefbfbdefbfbd"
===DONE===
