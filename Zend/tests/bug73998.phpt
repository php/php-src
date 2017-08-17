--TEST--
Bug #73998 (array_key_exists fails on arrays created by get_object_vars)
--DESCRIPTION--
Properties of objects with numeric names should be accessible
--FILE--
<?php
$a = new stdClass;
$a->{1234} = "Numeric";
$a->a1234 = "String";

$properties = get_object_vars($a);
var_dump(array_key_exists(1234, $properties));
echo "Value: {$properties[1234]}\n";

?>
--EXPECT--
bool(true)
Value: Numeric

