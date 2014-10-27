--TEST--
testing hash returns string, different ops
--FILE--
<?php
class Foo {
    public function __hash() {
        return __CLASS__;
    }
}

$foo = new Foo();
$test = [
    $foo => true,
    "bar" => false,
];

var_dump($test);
var_dump($test[$foo]);
var_dump(isset($test[$foo]));
unset($test[$foo]);
var_dump($test);
var_dump(empty($test[$foo]));
$test[$foo] = 42;
$test[$foo]++;
var_dump($test);
?>
--EXPECT--	
array(2) {
  ["Foo"]=>
  bool(true)
  ["bar"]=>
  bool(false)
}
bool(true)
bool(true)
array(1) {
  ["bar"]=>
  bool(false)
}
bool(true)
array(2) {
  ["bar"]=>
  bool(false)
  ["Foo"]=>
  int(43)
}


