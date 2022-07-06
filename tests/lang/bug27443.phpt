--TEST--
Bug #27443 (defined() returns wrong type)
--FILE--
<?php
echo gettype(defined('test'));
?>
--EXPECT--
boolean
