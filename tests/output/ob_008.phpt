--TEST--
output buffering - ob_get_contents
--FILE--
<?php
ob_start();
echo "foo\n";
echo ob_get_contents();
?>
--EXPECT--
foo
foo
