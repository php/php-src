--TEST--
Testing basic exception type compiler pass with param
--FILE--
<?php
function test (int $param) throws Exception {
	return $param;
}
?>
OK
--EXPECT--
OK

