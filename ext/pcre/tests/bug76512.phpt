--TEST--
Bug #76512 (\w no longer includes unicode characters)
--FILE--
<?php
var_dump(preg_match('/\w/u', 'ä'));
?>
--EXPECT--
int(1)
