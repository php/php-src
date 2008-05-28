--TEST--
Testing stream_encoding()
--FILE--
<?php

stream_encoding(fopen(__FILE__, 'r'));
print "Done";

?>
--EXPECT--
Done
