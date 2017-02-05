--TEST--
Testing basic exception type compiler pass
--FILE--
<?php
function test () throws Exception {}
?>
OK
--EXPECT--
OK

