--TEST--
Internal enums can be safely read from reflection
--EXTENSIONS--
zend_test
--FILE--
<?php

$r = new ReflectionEnum("ZendTestStringBacked");
var_dump($r->getCases());
var_dump($r->getConstant("FIRST") === ZendTestStringBacked::FIRST);
var_dump($r->getConstants()["FIRST"] === ZendTestStringBacked::FIRST);

?>
--EXPECTF--
array(2) {
  [0]=>
  object(ReflectionEnumBackedCase)#%d (2) {
    ["name"]=>
    string(5) "FIRST"
    ["class"]=>
    string(20) "ZendTestStringBacked"
  }
  [1]=>
  object(ReflectionEnumBackedCase)#%d (2) {
    ["name"]=>
    string(6) "SECOND"
    ["class"]=>
    string(20) "ZendTestStringBacked"
  }
}
bool(true)
bool(true)
