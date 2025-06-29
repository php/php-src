--TEST--
IntlTimeZone::getCanonicalID: refs test
--EXTENSIONS--
intl
--FILE--
<?php
class Test {
    public string $prop = "a";
}
$test = new Test;
$ref =& $test->prop;
print_R(intltz_get_canonical_id('Portugal', $ref));
var_dump($test);
?>
--EXPECTF--
Deprecated: Assigning bool to typed property which is implicitly converted to type string is deprecated in %s on line %d
Europe/Lisbonobject(Test)#1 (1) {
  ["prop"]=>
  &string(1) "1"
}
