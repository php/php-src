--TEST--
Including a file in the current script directory from an included function
--FILE--
<?php
require_once 'include_files/function.inc';
test();
?>
--EXPECT--
Included!
