--TEST--
Bug #67249 (printf out-of-bounds read)
--FILE--
<?php
var_dump(sprintf("%'", "foo"));
?>
--EXPECT--
string(0) ""
