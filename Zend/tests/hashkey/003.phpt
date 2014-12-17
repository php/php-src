--TEST--
testing hash returns true
--FILE--
<?php
class Foo {
    public function __hash() {
        return true;
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
  [1]=>
  bool(true)
}


