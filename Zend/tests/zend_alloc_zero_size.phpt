--TEST--
Zero-size allocations with the tracked allocator
--EXTENSIONS--
zend_test
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
var_dump(pack(""));
var_dump(vsprintf("", []));
?>
--EXPECT--
string(0) ""
string(0) ""
