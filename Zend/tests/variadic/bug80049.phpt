--TEST--
Bug #80049: Memleak when coercing integers to string via variadic argument
--FILE--
<?php
function coerceToString(string ...$strings) {
    var_dump($strings);
}
coerceToString(...[123]);
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "123"
}
