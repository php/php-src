--TEST--
mysqli bind_param/bind_result date
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");
		
	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_result");
  	mysqli_query($link,"CREATE TABLE test_bind_result(c1 date, c2 time, 
                                                        c3 timestamp(14), 
                                                        c4 year, 
                                                        c5 datetime, 
                                                        c6 timestamp(4), 
                                                        c7 timestamp(6))");

	$stmt = mysqli_prepare($link, "INSERT INTO test_bind_result VALUES (?,?,?,?,?,?,?)");
	mysqli_bind_param($stmt, &$d1, MYSQLI_BIND_STRING,
				 &$d2, MYSQLI_BIND_STRING,
				 &$d3, MYSQLI_BIND_STRING,
				 &$d4, MYSQLI_BIND_STRING,
				 &$d5, MYSQLI_BIND_STRING,
				 &$d6, MYSQLI_BIND_STRING,
				 &$d7, MYSQLI_BIND_STRING);

  	$d1 = '2002-01-02';
	$d2 = '12:49:00';
	$d3 = '2002-01-02 17:46:59';
	$d4 = 2010;
	$d5 ='2010-07-10';
	$d6 = '2020';
	$d7 = '1999-12-29';

	mysqli_execute($stmt);
	mysqli_stmt_close($stmt);

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_result");

  	mysqli_bind_result($stmt,&$c1, &$c2, &$c3, &$c4, &$c5, &$c6, &$c7);
	
 	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
array(7) {
  [0]=>
  string(10) "2002-01-02"
  [1]=>
  string(8) "12:49:00"
  [2]=>
  string(19) "2002-01-02 17:46:59"
  [3]=>
  int(2010)
  [4]=>
  string(19) "2010-07-10 00:00:00"
  [5]=>
  string(0) ""
  [6]=>
  string(19) "1999-12-29 00:00:00"
}
