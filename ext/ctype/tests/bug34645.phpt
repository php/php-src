--TEST--
Bug #34645 (ctype corrupts memory when validating large numbers)
--EXTENSIONS--
ctype
--FILE--
<?php
$id = 394829384;
var_dump(ctype_digit($id));
var_dump($id);
?>
--EXPECTF--
Deprecated: ctype_digit(): Argument of type int will be interpreted as string in the future in %s on line %d
bool(true)
int(394829384)
