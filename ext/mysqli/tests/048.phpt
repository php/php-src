--TEST--
mysqli bind_result (OO-Style) 
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = mysqli_connect("localhost", $user, $passwd);

	$mysql->select_db("test");		
	$mysql->query("DROP TABLE IF EXISTS test_fetch_null");

  	$mysql->query("CREATE TABLE test_fetch_null(col1 tinyint, col2 smallint,
                                                       col3 int, col4 bigint, 
                                                       col5 float, col6 double,
                                                       col7 date, col8 time, 
                                                       col9 varbinary(10), 
                                                       col10 varchar(50),
                                                       col11 char(20))");
  
	$mysql->query("INSERT INTO test_fetch_null(col1,col10, col11) VALUES(1,'foo1', 1000),(2,'foo2', 88),(3,'foo3', 389789)");

	$stmt = $mysql->prepare("SELECT col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11 from test_fetch_null");
	$stmt->bind_result(&$c1, &$c2, &$c3, &$c4, &$c5, &$c6, &$c7, &$c8, &$c9, &$c10, &$c11); 
	$stmt->execute();

	$stmt->fetch();

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$c10,$c11);

	var_dump($test);

	$stmt->close();
	$mysql->close();
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
