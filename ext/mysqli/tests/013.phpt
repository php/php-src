--TEST--
mysqli fetch mixed / mysql_query 
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_result");

	mysqli_query($link,"CREATE TABLE test_bind_result(c1 tinyint, c2 smallint, 
                                                        c3 int, c4 bigint, 
                                                        c5 decimal(4,2), c6 double,
                                                        c7 varbinary(10), 
                                                        c8 varchar(10))");

  	mysqli_query($link,"INSERT INTO test_bind_result VALUES(120,2999,3999,54,
                                                              2.6,58.89,
                                                              '206','6.7')");
	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_result");

	$c = array(0,0,0,0,0,0,0,0);
	mysqli_bind_result($stmt, &$c[0], &$c[1], &$c[2], &$c[3], &$c[4], &$c[5], &$c[6], &$c[7]);
	mysqli_execute($stmt);
	mysqli_fetch($stmt); 
	mysqli_fetch($stmt);  
	mysqli_stmt_close($stmt);

	$result = mysqli_query($link, "select * from test_bind_result");
	$d = mysqli_fetch_row($result); 
	mysqli_free_result($result);

	$test = "";
	for ($i=0; $i < count($c); $i++)
		$test .= ($c[0] == $d[0]) ? "1" : "0";

	var_dump($test);

	mysqli_close($link);
?>
--EXPECT--
string(8) "11111111"
