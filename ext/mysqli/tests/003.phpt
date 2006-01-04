--TEST--
mysqli connect
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, "test");

	mysqli_query($link, "SET sql_mode=''");
		
	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_result");
  	mysqli_query($link,"CREATE TABLE test_bind_result(c1 date, c2 time, 
                                                        c3 timestamp(14), 
                                                        c4 year, 
                                                        c5 datetime, 
                                                        c6 timestamp(4), 
                                                        c7 timestamp(6))");

  	mysqli_query($link,"INSERT INTO test_bind_result VALUES('2002-01-02',
                                                              '12:49:00',
                                                              '2002-01-02 17:46:59', 
                                                              2010,
                                                              '2010-07-10', 
                                                              '2020','1999-12-29')");


	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_result");
  	mysqli_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7);
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
  string(19) "0000-00-00 00:00:00"
  [6]=>
  string(19) "1999-12-29 00:00:00"
}
