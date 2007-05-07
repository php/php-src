--TEST--
Bug #32833 (Invalid opcode with $a[] .= '')
--FILE--
<?php 
$test = array();
$test[] .= "ok\n";
echo $test[0];
?>
--EXPECT--
ok
