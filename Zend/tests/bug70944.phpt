--TEST--
Bug #70944 (try{ } finally{} can create infinite chains of exceptions)
--FILE--
<?php
try {
	$e = new Exception("Foo");
	try {
		throw  new Exception("Bar", 0, $e);
	} finally {
		throw $e;
	}
} catch (Exception $e) {
	var_dump($e->getMessage());
}

try {
	$e = new Exception("Foo");
	try {
		throw new Exception("Bar", 0, $e);
	} finally {
		throw new Exception("Dummy", 0, $e);
	}
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECT--
string(3) "Foo"
string(5) "Dummy"
