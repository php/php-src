--TEST--
bug #71428.2: inheritance of ye olde dynamic interfaces
--SKIPIF--
<?php if (!extension_loaded('pdo')) die("skip PDO is not available"); ?>
--FILE--
<?php
interface StatementInterface {
	public function fetch($first = null, $second, $third);
}

class Statement extends PDOStatement implements StatementInterface {}

interface StatementInterface1 {
	public function fetch($first = null, $second = null, $third = null);
}

class Statement1 extends PDOStatement implements StatementInterface1 {}

echo "ok";
?>
--EXPECT--
ok
