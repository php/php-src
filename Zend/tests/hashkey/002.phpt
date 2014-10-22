--TEST--
testing hashKey inheritance
--FILE--
<?php
class Foo {
    public function __hashKey() {
        return static::class;
    }
}

class Bar extends Foo {}

$bar = new Bar();
$test = [
    $bar => true
];

var_dump($test);
?>
--EXPECT--	
array(1) {
  ["Bar"]=>
  bool(true)
}


