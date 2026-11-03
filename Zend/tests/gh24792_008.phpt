--TEST--
GH-24792: Dimension write on the result of a destructuring assignment with references
--FILE--
<?php
$arr = [[1]];
([&$x] = $arr)[0] = 5;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
