--TEST--
Testing normalization in array key
--FILE--
<?php 

$GLOBALS["\u212B"] = ' 승인 ';

var_dump(isset($GLOBALS['Å']));

?>
--EXPECT--
bool(true)
