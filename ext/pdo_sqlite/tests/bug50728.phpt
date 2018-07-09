--TEST--
Bug #50728 (All PDOExceptions hardcode 'code' property to 0)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
try {
	$a = new PDO("sqlite:/this/path/should/not/exist.db");
} catch (PDOException $e) {
	var_dump($e->getCode());
}
?>
--EXPECT--
int(14)
