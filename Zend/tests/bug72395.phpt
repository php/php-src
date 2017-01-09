--TEST--
Bug #72395 (list() regression)
--FILE--
<?php
list(,,$a,,$b,) = array(1, 2, 3, 4, 5, 6);
var_dump($a, $b);
?>
--EXPECT--
int(3)
int(5)
