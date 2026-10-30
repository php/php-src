--TEST--
GH-19303 (Unpacking empty packed array into uninitialized array causes assertion failure)
--FILE--
<?php
$a = [0];
unset($a[0]);
var_dump([...$a]);
?>
--EXPECT--
array(0) {
}
