--TEST--
Named params in attributes
--FILE--
<?php

#[Attribute]
class MyAttribute {
    public function __construct(
        public $a = 'a',
        public $b = 'b',
        public $c = 'c',
    ) {}
}

#[MyAttribute('A', c: 'C')]
class Test1 {}

#[MyAttribute('A', a: 'C')]
class Test2 {}

$attr = (new ReflectionClass(Test1::class))->getAttributes()[0];
var_dump($attr->getName());
var_dump($attr->getArguments());
var_dump($attr->newInstance());

$attr = (new ReflectionClass(Test2::class))->getAttributes()[0];
try {
    var_dump($attr->newInstance());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(11) "MyAttribute"
array(2) {
  [0]=>
  string(1) "A"
  ["c"]=>
  string(1) "C"
}
object(MyAttribute)#1 (3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(1) "b"
  ["c"]=>
  string(1) "C"
}
Named parameter $a overwrites previous argument
