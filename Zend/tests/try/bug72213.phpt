--TEST--
Bug #72213 (Finally leaks on nested exceptions)
--FILE--
<?php
function test() {
	try {
		throw new Exception('a');
	} finally {
		try {
			throw new Exception('b');
		} finally {
		}
	}
}

try {
	test();
} catch (Exception $e) {
	var_dump($e->getMessage());
	var_dump($e->getPrevious()->getMessage());
}
?>
--EXPECT--
string(1) "b"
string(1) "a"
