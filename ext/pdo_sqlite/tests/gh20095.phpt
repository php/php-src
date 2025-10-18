--TEST--
GH-20095: Incorrect class name in deprecation message for PDO mixins
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class Foo extends PDO {
	private function test() {
		echo "foo";
	}

	public function register() {
		$this->sqliteCreateFunction('my_test', [$this, "test"]);
	}
}

$pdo = new Foo('sqlite::memory:');
$pdo->register();
$pdo->query("SELECT my_test()");

?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateFunction() is deprecated since 8.5, use Pdo\Sqlite::createFunction() instead in %s on line %d
foo
