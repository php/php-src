--TEST--
output buffering - ob_get_flush
--FILE--
<?php
ob_start();
echo "foo\n";
var_dump(ob_get_flush());
?>
--EXPECT--
foo
string(4) "foo
"
