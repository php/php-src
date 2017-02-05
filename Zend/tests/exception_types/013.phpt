--TEST--
Testing exception type executor with return and param pass
--FILE--
<?php
function test (int $param) : int throws Exception {
	return $param;
}

try {
	test(10);
} catch (FancyException $ex) {}
?>
OK
--EXPECT--
OK
