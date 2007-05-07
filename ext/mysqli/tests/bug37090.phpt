--TEST--
Bug #37090 (mysqli_set_charset return code)
--SKIPIF--
<?php require_once('skipif.inc'); 
if (!function_exists('mysqli_set_charset')) {
	die('skip mysqli_set_charset() not available');
}
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd);

	$cs = array();
	$cs[] = $mysql->set_charset("latin5");
	$cs[] = $mysql->character_set_name();

	$cs[] = $mysql->set_charset("utf8");
	$cs[] = $mysql->character_set_name();

	$cs[] = $mysql->set_charset("notdefined");
	$cs[] = $mysql->character_set_name();

	var_dump($cs);
?>
--EXPECT--
array(6) {
  [0]=>
  bool(true)
  [1]=>
  string(6) "latin5"
  [2]=>
  bool(true)
  [3]=>
  string(4) "utf8"
  [4]=>
  bool(false)
  [5]=>
  string(4) "utf8"
}
