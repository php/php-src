--TEST--
Testing basic exception type compiler pass with return
--FILE--
<?php
function test () : int throws Exception {
	return 1;
}
?>
OK
--EXPECT--
OK

