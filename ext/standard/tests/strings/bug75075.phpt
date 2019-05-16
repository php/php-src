--TEST--
Bug #75075 (unpack with X* causes infinity loop)
--FILE--
<?php
var_dump(unpack("X*", ""));
?>
--EXPECTF--
Warning: unpack(): Type X: '*' ignored in %sbug75075.php on line %d
array(0) {
}
