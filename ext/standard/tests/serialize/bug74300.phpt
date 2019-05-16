--TEST--
Bug #74300 (unserialize() for float nvexp part)
--INI--
serialize_precision=10
--FILE--
<?php
var_dump(unserialize('d:2e+2;'));
var_dump(unserialize('d:2e++2;'));
?>
===DONE===
--EXPECTF--
float(200)

Notice: unserialize(): Error at offset 0 of 8 bytes in %s on line %d
bool(false)
===DONE===
