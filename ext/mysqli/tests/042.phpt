--TEST--
mysqli_fetch_object
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
  	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 smallint unsigned,
                                                     c2 smallint unsigned,
                                                     c3 smallint,
                                                     c4 smallint,
                                                     c5 smallint,
                                                     c6 smallint unsigned,
                                                     c7 smallint)");

	$stmt = mysqli_prepare($link, "INSERT INTO test_bind_fetch VALUES (?,?,?,?,?,?,?)");
	mysqli_bind_param($stmt, &$c1,MYSQLI_BIND_INT,&$c2,MYSQLI_BIND_INT,&$c3,MYSQLI_BIND_INT,
				 &$c4,MYSQLI_BIND_INT,&$c5,MYSQLI_BIND_INT,&$c6,MYSQLI_BIND_INT,
				 &$c7, MYSQLI_BIND_INT);

	$c1 = -23;
	$c2 = 35999;
	$c3 = NULL;
	$c4 = -500;
	$c5 = -9999999;
	$c6 = -0;
	$c7 = 0;

	mysqli_execute($stmt);
	mysqli_stmt_close($stmt);

	$result = mysqli_query($link, "SELECT * FROM test_bind_fetch");
	$test = mysqli_fetch_object($result);
	mysqli_free_result($result);

	var_dump($test);

	mysqli_close($link);
?>
--EXPECT--
object()(7) {
  ["c1"]=>
  string(1) "0"
  ["c2"]=>
  string(5) "35999"
  ["c3"]=>
  NULL
  ["c4"]=>
  string(4) "-500"
  ["c5"]=>
  string(6) "-32768"
  ["c6"]=>
  string(1) "0"
  ["c7"]=>
  string(1) "0"
}
