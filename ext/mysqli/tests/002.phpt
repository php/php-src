--TEST--
mysqli bind_result 1 
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");		
	$rc = mysqli_query($link,"DROP TABLE IF EXISTS test_fetch_null");

  	$rc = mysqli_query($link,"CREATE TABLE test_fetch_null(col1 tinyint, col2 smallint,
                                                       col3 int, col4 bigint, 
                                                       col5 float, col6 double,
                                                       col7 date, col8 time, 
                                                       col9 varbinary(10), 
                                                       col10 varchar(50),
                                                       col11 char(20))");
  
	$rc = mysqli_query($link,"INSERT INTO test_fetch_null(col1,col10, col11) VALUES(1,'foo1', 1000),(2,'foo2', 88),(3,'foo3', 389789)");

	$stmt = mysqli_prepare($link, "SELECT col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11 from test_fetch_null");
	mysqli_bind_result($stmt, &$c1, &$c2, &$c3, &$c4, &$c5, &$c6, &$c7, &$c8, &$c9, &$c10, &$c11); 
	mysqli_execute($stmt);

	mysqli_fetch($stmt);

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$c10,$c11);

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
array(11) {
  [0]=>
  int(1)
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  NULL
  [6]=>
  NULL
  [7]=>
  NULL
  [8]=>
  NULL
  [9]=>
  string(4) "foo1"
  [10]=>
  string(4) "1000"
}
