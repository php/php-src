--TEST--
mysqli_bind_result (SHOW)
--SKIPIF--
<?php	
	require_once('skipif.inc');
	require_once('skipifemb.inc'); 

	include "connect.inc";
	$link = mysqli_connect($host, $user, $passwd);


	$stmt = mysqli_prepare($link, "SHOW VARIABLES LIKE 'port'");
	mysqli_execute($stmt);

	if (!$stmt->field_count) {
		printf("skip SHOW command is not supported in prepared statements.");
	}
	$stmt->close();
	mysqli_close($link);
?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	$stmt = mysqli_prepare($link, "SHOW VARIABLES LIKE 'port'");
	mysqli_execute($stmt);

	mysqli_bind_result($stmt, $c1, $c2); 	
	mysqli_fetch($stmt);
	mysqli_stmt_close($stmt);	
	$test = array ($c1,$c2);

	var_dump($test);

	mysqli_close($link);
?>
--EXPECT--
array(2) {
  [0]=>
  string(4) "port"
  [1]=>
  string(4) "3306"
}
