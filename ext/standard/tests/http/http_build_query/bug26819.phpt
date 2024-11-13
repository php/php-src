--TEST--
Bug #26819 (http_build_query() crash on empty output)
--FILE--
<?php
$a = array();
var_dump(http_build_query($a));
?>
--EXPECT--
string(0) ""
