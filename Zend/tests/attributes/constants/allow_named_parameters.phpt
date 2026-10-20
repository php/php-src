--TEST--
Verify that named parameters can be passed to attributes on constants
--FILE--
<?php

#[Attribute]
class MyAttribute {
    public function __construct($first, $second) {
        echo "first: $first\n";
        echo "second: $second\n";
    }
}

#[MyAttribute(second: "bar", first: "foo")]
const EXAMPLE = 'ignored';

$ref = new ReflectionConstant('EXAMPLE');
$attribs = $ref->getAttributes();
var_dump($attribs);
var_dump($attribs[0]->getArguments());
$attribs[0]->newInstance();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
}
array(2) {
  ["second"]=>
  string(3) "bar"
  ["first"]=>
  string(3) "foo"
}
first: foo
second: bar
