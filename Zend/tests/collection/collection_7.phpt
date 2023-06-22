--TEST--
Collection: Syntax Errors
--FILE--
<?php
collection(Dict) Articles(int => Article => three)
{
}
?>
--EXPECTF--
Fatal error: Invalid collection data types signature in %scollection_7.php on line %d
