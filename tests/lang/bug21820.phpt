--TEST--
Bug #21820 ("$arr['foo']" generates bogus E_NOTICE, should be E_PARSE)
--FILE--
<?php

error_reporting(E_ALL);

$arr = array('foo' => 'bar');
echo "$arr['foo']";

?>
--EXPECTF--
Parse error: syntax error, unexpected T_ENCAPSED_AND_WHITESPACE, expecting T_STRING or T_VARIABLE or T_NUM_STRING in %sbug21820.php on line %d
