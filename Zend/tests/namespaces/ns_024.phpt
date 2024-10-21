--TEST--
024: __NAMESPACE__ constant out of namespace
--FILE--
<?php
var_dump(__NAMESPACE__);
?>
--EXPECT--
string(0) ""
