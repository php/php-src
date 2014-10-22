--TEST--
testing hashKey returns string
--FILE--
<?php
class Foo {
    public function __hashKey() {
        return __CLASS__;
    }
}

$foo = new Foo();
$test = [
    $foo => true
];

var_dump($test);
?>
--EXPECT--	
array(1) {
  ["Foo"]=>
  bool(true)
}


