--TEST--
testing hash returns null
--FILE--
<?php
class Foo {
    public function __hash() {
        return null;
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
  [""]=>
  bool(true)
}


