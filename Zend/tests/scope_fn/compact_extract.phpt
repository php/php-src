--TEST--
compact() and extract() inside scope function
--FILE--
<?php
function test_compact() {
    $a = 1;
    $b = 2;
    $fn = fn() { return compact('a', 'b'); };
    var_dump($fn());
}
test_compact();

function test_extract() {
    $fn = fn() { extract(['x' => 10, 'y' => 20]); };
    $fn();
    var_dump($x, $y);
}
test_extract();
?>
--EXPECT--
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
int(10)
int(20)
