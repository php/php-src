--TEST--
Bug #74670: Integer Underflow when unserializing GMP and possible other classes
--EXTENSIONS--
gmp
--FILE--
<?php
$str = 'C:3:"GMP":4:{s:6666666666:""}';
var_dump(unserialize($str));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 17 of 29 bytes in %s on line %d
bool(false)
