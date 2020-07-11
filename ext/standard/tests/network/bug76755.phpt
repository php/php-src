--TEST--
Bug #76755 (setcookie does not accept "double" type for expire time)
--FILE--
<?php
var_dump(setcookie('name', 'value', (float)(time() + 1296000)));
?>
--EXPECT--
bool(true)
