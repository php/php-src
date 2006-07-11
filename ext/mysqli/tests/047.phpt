--TEST--
mysqli_get_metadata
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
	mysqli_query($link, "CREATE TABLE test_affected (foo int, bar varchar(10) character set latin1)");

	mysqli_query($link, "INSERT INTO test_affected VALUES (1, 'Zak'),(2, 'Greant')");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_affected");
	mysqli_execute($stmt);
	$result = mysqli_get_metadata($stmt);

	echo "\n=== fetch_fields ===\n";
	var_dump(mysqli_fetch_fields($result));

	echo "\n=== fetch_field_direct ===\n";
	var_dump(mysqli_fetch_field_direct($result, 0));
	var_dump(mysqli_fetch_field_direct($result, 1));

	echo "\n=== fetch_field ===\n";
	while ($field = mysqli_fetch_field($result)) {
		var_dump($field);
	}
    
	print_r(mysqli_fetch_lengths($result));
    
	mysqli_free_result($result);


	mysqli_stmt_close($stmt);	
	mysqli_close($link);
?>
--EXPECTF--
=== fetch_fields ===
array(2) {
  [0]=>
  object(stdClass)#5 (11) {
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
    ["length"]=>
    int(11)
    ["charsetnr"]=>
    int(63)
    ["flags"]=>
    int(32768)
    ["type"]=>
    int(3)
    ["decimals"]=>
    int(0)
  }
  [1]=>
  object(stdClass)#6 (11) {
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
    ["length"]=>
    int(10)
    ["charsetnr"]=>
    int(8)
    ["flags"]=>
    int(0)
    ["type"]=>
    int(253)
    ["decimals"]=>
    int(0)
  }
}

=== fetch_field_direct ===
object(stdClass)#6 (11) {
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
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(32768)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
object(stdClass)#6 (11) {
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
  ["length"]=>
  int(10)
  ["charsetnr"]=>
  int(8)
  ["flags"]=>
  int(0)
  ["type"]=>
  int(253)
  ["decimals"]=>
  int(0)
}

=== fetch_field ===
object(stdClass)#6 (11) {
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
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(32768)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
object(stdClass)#5 (11) {
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
  ["length"]=>
  int(10)
  ["charsetnr"]=>
  int(8)
  ["flags"]=>
  int(0)
  ["type"]=>
  int(253)
  ["decimals"]=>
  int(0)
}