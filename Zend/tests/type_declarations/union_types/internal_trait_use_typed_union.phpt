--TEST--
Internal trait used typed property (union type)
--EXTENSIONS--
zend_test
--FILE--
<?php

class C {
    use _ZendTestTrait;
}

$o = new C();
var_dump($o);

$prop = new \ReflectionProperty(C::class, 'classUnionProp');
$union = $prop->getType();
$types = $union->getTypes();
var_dump($types, (string)$types[0], (string)$types[1]);

?>
===DONE===
--EXPECT--
object(C)#1 (1) {
  ["testProp"]=>
  NULL
  ["classUnionProp"]=>
  uninitialized(Traversable|Countable)
}
array(2) {
  [0]=>
  object(ReflectionNamedType)#4 (0) {
  }
  [1]=>
  object(ReflectionNamedType)#5 (0) {
  }
}
string(11) "Traversable"
string(9) "Countable"
===DONE===
