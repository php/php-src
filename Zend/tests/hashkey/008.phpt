--TEST--
testing hashKey returns double
--FILE--
<?php
class Foo {
    public function __hashKey() {
        return 10.1;
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
  [10]=>
  bool(true)
}


