--TEST--
Bug #30085 (When destructor terminates script)
--FILE--
<?php
class test { }

class test2 {

	public $test;

	public function __construct() {
		$this->test = new test;
	}

	public function __destruct() {
		exit("Bye");
	}

}

$test = new test2;
?>
--EXPECT--
Bye