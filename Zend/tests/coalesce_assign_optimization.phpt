--TEST--
Live range construction should not break if colesce assign branch is optimized away
--FILE--
<?php
function test() {
    $a[X] ??= Y;
    var_dump($a);
}
function test2(string $b, int $c) {
    $a[~$b] ??= $c;
}
define('X', 1);
define('Y', 2);
test();
test2("", 0);
?>
--EXPECT--
array(1) {
  [1]=>
  int(2)
}
