--TEST--
Bug #73847: Recursion when a variable is redefined as array
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    $a = 42;
    $a = array($a);
    var_dump($a);

    $a = 42;
    $a = array($a => 24);
    var_dump($a);

    $a = 42;
    $a = array($a, 24);
    var_dump($a);

    $a = 42;
    $a = array(24, $a);
    var_dump($a);
}
test();
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [42]=>
  int(24)
}
array(2) {
  [0]=>
  int(42)
  [1]=>
  int(24)
}
array(2) {
  [0]=>
  int(24)
  [1]=>
  int(42)
}
