--TEST--
Collection: Syntax Errors
--FILE--
<?php
collection(Dict) Articles<int => Article => three>
{
}
?>
--EXPECTF--
Fatal error: Invalid collection data types signature in %s on line %d
