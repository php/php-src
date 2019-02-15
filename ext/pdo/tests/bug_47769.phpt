--TEST--
PDO Common: Bug #47769 (Strange extends PDO)
--SKIPIF--
<?php
if (!extension_loaded("pdo_sqlite"))
	die("skip: PDO_SQLite not available");
?>
--FILE--
<?php

class test extends PDO
{
	protected function isProtected() {
		echo "this is a protected method.\n";
	}
	private function isPrivate() {
		echo "this is a private method.\n";
	}

    public function quote($str, $paramtype = NULL) {
    	$this->isProtected();
    	$this->isPrivate();
    	print $str ."\n";
	}
}

$test = new test('sqlite::memory:');
$test->quote('foo');
$test->isProtected();

?>
--EXPECTF--
this is a protected method.
this is a private method.
foo

Fatal error: Uncaught Error: Call to protected method test::isProtected() from context '' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
