--TEST--
Bug #32833 Invalid opcode 
--FILE--
<?php 
$test = array();
$test[] .= "ok\n";
echo $test[0];
?>
--EXPECT--
ok
