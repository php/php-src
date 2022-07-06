--TEST--
output buffering - ob_flush
--FILE--
<?php
ob_start();
echo "foo\n";
ob_flush();
echo "bar\n";
ob_flush();
?>
--EXPECT--
foo
bar
