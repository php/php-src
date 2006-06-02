--TEST--
output buffering - ob_start
--FILE--
<?php
ob_start();
echo "foo\n";
?>
--EXPECT--
foo
