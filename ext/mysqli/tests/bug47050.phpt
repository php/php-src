--TEST--
Bug #47050 (mysqli_poll() modifies improper variables)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (!defined("MYSQLI_ASYNC")) {
	die("skip mysqlnd only");
}
?>
--FILE--
<?php
	include ("connect.inc");

	$link1 = my_mysqli_connect($host, $user, $passwd, null, $port, $socket);
	mysqli_select_db($link1, $db);

	$link1->query("SELECT 'test'", MYSQLI_ASYNC);
	$all_links = array($link1);
	$links = $errors = $reject = $all_links;
	mysqli_poll($links, $errors, $reject, 1);

	echo "links: ",     sizeof($links), "\n";
	echo "errors: ",    sizeof($errors), "\n";
	echo "reject: ",    sizeof($reject), "\n";
	echo "all_links: ", sizeof($all_links), "\n";

	$link1->close();
?>
--EXPECT--
links: 1
errors: 0
reject: 0
all_links: 1
