--TEST--
Bug #36949 (invalid internal mysqli objects dtor)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "connect.inc";

class A {

	private $mysqli;

	public function __construct() {
		global $user, $host, $passwd;
		$this->mysqli = new mysqli($host, $user, $passwd);
		$result = $this->mysqli->query("SELECT NOW() AS my_time FROM DUAL");
		$row = $result->fetch_object();
		echo $row->my_time."<br>\n";
		$result->close();
	}

	public function __destruct() {
		$this->mysqli->close();
	}
}

class B {

	private $mysqli;

	public function __construct() {
		global $user, $host, $passwd;
		$this->mysqli = new mysqli($host, $user, $passwd);
		$result = $this->mysqli->query("SELECT NOW() AS my_time FROM DUAL");
		$row = $result->fetch_object();
		echo $row->my_time."<br>\n";
		$result->close();
	}

	public function __destruct() {
		$this->mysqli->close();
	}
}

$A = new A();
$B = new B();
?>
--EXPECTF--
%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d<br>
%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d<br>
