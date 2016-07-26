--TEST--
Bug #70983 - line-comment and close tag
--FILE--
<?php
$a = 'string';
// ?>
var_dump($a);
echo "Done";
?>
--EXPECT--	
string(6) "string"
Done
