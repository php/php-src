--TEST--
Bug #21820 ("$arr['foo']" generates bogus E_NOTICE, should be E_PARSE)
--FILE--
<?php

error_reporting(E_ALL);

$arr = array('foo' => 'bar');
echo "$arr['foo']";

?>
--EXPECTF--
Parse error: parse error, expecting `T_STRING' or `T_VARIABLE' or `T_NUM_STRING' in %s on line %d
