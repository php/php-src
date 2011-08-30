--TEST--
Bug #21820 ("$arr['foo']" generates bogus E_NOTICE, should be E_PARSE)
--FILE--
<?php

error_reporting(E_ALL);

$arr = array('foo' => 'bar');
echo "$arr['foo']";

?>
--EXPECTF--
Parse error: syntax error, unexpected "quoted-string and whitespace (T_ENCAPSED_AND_WHITESPACE)", expecting "identifier (T_STRING)" or "variable (T_VARIABLE)" or "number (T_NUM_STRING)" in %sbug21820.php on line %d
