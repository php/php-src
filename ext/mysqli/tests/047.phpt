--TEST--
mysqli_prepare_result
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
	mysqli_query($link, "CREATE TABLE test_affected (foo int, bar varchar(10))");

	mysqli_query($link, "INSERT INTO test_affected VALUES (1, 'Zak'),(2, 'Greant')");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_affected");
	mysqli_execute($stmt);
	$result = mysqli_prepare_result($stmt);

	$fields = mysqli_fetch_fields($result);
	mysqli_free_result($result);

	var_dump($fields);

	mysqli_stmt_close($stmt);	
	mysqli_close($link);
?>
--EXPECTF--
array(2) {
  [0]=>
  object(stdClass)(9) {
    ["name"]=>
    string(3) "foo"
    ["orgname"]=>
    string(3) "foo"
    ["table"]=>
    string(13) "test_affected"
    ["orgtable"]=>
    string(13) "test_affected"
    ["def"]=>
    string(0) ""
    ["max_length"]=>
    int(0)
    ["flags"]=>
    int(32768)
    ["type"]=>
    int(3)
    ["decimals"]=>
    int(0)
  }
  [1]=>
  object(stdClass)(9) {
    ["name"]=>
    string(3) "bar"
    ["orgname"]=>
    string(3) "bar"
    ["table"]=>
    string(13) "test_affected"
    ["orgtable"]=>
    string(13) "test_affected"
    ["def"]=>
    string(0) ""
    ["max_length"]=>
    int(0)
    ["flags"]=>
    int(0)
    ["type"]=>
    int(253)
    ["decimals"]=>
    int(0)
  }
}
