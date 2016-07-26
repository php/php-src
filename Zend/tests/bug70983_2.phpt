--TEST--
Bug #70983 - line-comment and close tag
--FILE--
<?php $a = 'string'; // ?>leftover
var_dump($a);
echo "Done";
?>
--EXPECT--	
leftover
var_dump($a);
echo "Done";
?>
