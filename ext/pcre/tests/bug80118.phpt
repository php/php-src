--TEST--
Bug #80118 (Erroneous whitespace match with JIT only)
--FILE--
<?php
preg_match('~[^\p{Han}\p{Z}]~u', '     ', $matches);
var_dump($matches);
?>
--EXPECT--
array(0) {
}
