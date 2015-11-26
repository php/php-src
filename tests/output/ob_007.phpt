--TEST--
output buffering - ob_get_clean
--FILE--
<?php
ob_start();
echo "foo\n";
var_dump(ob_get_clean());
?>
--EXPECT--
string(4) "foo
"
