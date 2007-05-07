--TEST--
Bug #31668 (multi_query works exactly every other time (multi_query was global, now per connection))
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");
	$mysql->multi_query('SELECT 1;SELECT 2');
	do {
		$res = $mysql->store_result();	
		if ($mysql->errno == 0) {
			while ($arr = $res->fetch_assoc()) {
				var_dump($arr);
			}
			$res->free();
		}
	} while ($mysql->next_result());
	var_dump($mysql->error, __LINE__);
	$mysql->close();

	$mysql = new mysqli($host, $user, $passwd, "test");
	$mysql->multi_query('SELECT 1;SELECT 2');
	do {
		$res = $mysql->store_result();	
		if ($mysql->errno == 0) {
			while ($arr = $res->fetch_assoc()) {
				var_dump($arr);
			}
			$res->free();
		}
	} while ($mysql->next_result());
	var_dump($mysql->error, __LINE__);
?>
--EXPECTF--
array(1) {
  [1]=>
  string(1) "1"
}
array(1) {
  [2]=>
  string(1) "2"
}
string(0) ""
int(%d)
array(1) {
  [1]=>
  string(1) "1"
}
array(1) {
  [2]=>
  string(1) "2"
}
string(0) ""
int(%d)
