--TEST--
function test: mysqli_num_fields() 2
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_real_query($link, "SHOW VARIABLES");

	if (mysqli_field_count($link)) {
		$result = mysqli_store_result($link);
		$num = mysqli_num_fields($result);
		mysqli_free_result($result);
	}

	var_dump($num);

	mysqli_close($link);
?>
--EXPECT--
int(2)
