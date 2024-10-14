--TEST--
Bug #79948: Exit in auto-prepended file does not abort PHP execution
--INI--
auto_prepend_file={PWD}/bug79948.inc
--FILE--
<?php
echo "Should not be executed.\n";
?>
--EXPECT--
Exiting...
