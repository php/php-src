--TEST--
059: Constant arrays
--FILE--
<?php
const C = array();
var_dump(C);
--EXPECTF--
array(0) {
}
