--TEST--
Bug #60321 (ob_get_status(true) no longer returns an array when buffer is empty)
--FILE--
<?php
$return = ob_get_status(true);
var_dump($return);
--EXPECT--
array(0) {
}
