--TEST--
Bug #49027 (mysqli_options() doesn't work when using mysqlnd)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include ("connect.inc");

	$link=mysqli_init();
	if (!mysqli_options($link, MYSQLI_INIT_COMMAND, "SELECT 1")){
		echo "Broken 2!\n";
	}
	if (!mysqli_options($link, MYSQLI_INIT_COMMAND, "SELECT 13")){
		echo "Broken 2!\n";
	}

	require('table_real_connect.inc');

	var_dump($link->query("SELECT 42")->fetch_row());
?>
--EXPECTF--
array(1) {
  [0]=>
  %unicode|string%(2) "42"
}
