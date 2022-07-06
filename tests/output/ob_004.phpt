--TEST--
output buffering - ob_clean
--FILE--
<?php
ob_start();
echo "foo\n";
ob_clean();
echo "bar\n";
?>
--EXPECT--
bar
