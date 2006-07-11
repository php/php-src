--TEST--
mysqli multi_query, next_result, more_results
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
		if ($mysql->more_results()) {
			echo "---\n";
		}
	} while ($mysql->next_result());
	$mysql->close();
?>
--EXPECT--
array(1) {
  [1]=>
  string(1) "1"
}
---
array(1) {
  [2]=>
  string(1) "2"
}
