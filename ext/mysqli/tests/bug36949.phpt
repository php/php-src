--TEST--
Bug #36949 (invalid internal mysqli objects dtor)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once("connect.inc");
class A {

	private $mysqli;

	public function __construct() {
		global $user, $host, $passwd, $db, $port, $socket;
		$this->mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket);
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
		global $user, $host, $passwd, $db, $port, $socket;
		$this->mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket);
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
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS my_time"))
	printf("[002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d<br>
%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d<br>
