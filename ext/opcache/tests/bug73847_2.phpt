--TEST--
Bug #73847: Recursion when a variable is redefined as array
--EXTENSIONS--
opcache
--FILE--
<?php
function test(int $a) {
    $a = array($a);
    var_dump($a);
}
test(42);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
