--TEST--
mysqli_prepare_result
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_store_result");
  	mysqli_query($link,"CREATE TABLE test_store_result (a int)");

	mysqli_query($link, "INSERT INTO test_store_result VALUES (1),(2),(3)");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_store_result");
	mysqli_execute($stmt);

	/* this should produce an out of sync error */
	if ($result = mysqli_query($link, "SELECT * FROM test_store_result")) {
		mysqli_free_result($result);
		printf ("Query ok\n");
	}
	mysqli_stmt_close($stmt);

	$stmt = mysqli_prepare($link, "SELECT * FROM test_store_result");
	mysqli_execute($stmt);
	$result1 = mysqli_prepare_result($stmt);
	mysqli_stmt_store_result($stmt);

	printf ("Rows: %d\n", mysqli_stmt_affected_rows($stmt));

	/* this should show an error, cause results are not buffered */
	if ($result = mysqli_query($link, "SELECT * FROM test_store_result")) {
		$row = mysqli_fetch_row($result);
		mysqli_free_result($result);
	} 
	

	var_dump($row);	

	mysqli_free_result($result1);
	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
Rows: 3
array(1) {
  [0]=>
  string(1) "1"
}
