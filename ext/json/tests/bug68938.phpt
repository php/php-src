--TEST--
Bug #68938 (json_decode() decodes empty string without indicating error)
--FILE--
<?php
json_decode("");
var_dump(json_last_error());
?>
--EXPECT--
int(4)
