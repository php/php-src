--TEST--
mysqli_get_metadata
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	mysqli_select_db($link, $db);

	mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
	mysqli_query($link, "CREATE TABLE test_affected (foo int, bar varchar(10) character set latin1) ENGINE=" . $engine);

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
	mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_affected"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
=== fetch_fields ===
array(2) {
  [0]=>
  object(stdClass)#5 (11) {
    [%u|b%"name"]=>
    %unicode|string%(3) "foo"
    [%u|b%"orgname"]=>
    %unicode|string%(3) "foo"
    [%u|b%"table"]=>
    %unicode|string%(13) "test_affected"
    [%u|b%"orgtable"]=>
    %unicode|string%(13) "test_affected"
    [%u|b%"def"]=>
    %unicode|string%(0) ""
    [%u|b%"max_length"]=>
    int(0)
    [%u|b%"length"]=>
    int(%d)
    [%u|b%"charsetnr"]=>
    int(%d)
    [%u|b%"flags"]=>
    int(32768)
    [%u|b%"type"]=>
    int(3)
    [%u|b%"decimals"]=>
    int(0)
  }
  [1]=>
  object(stdClass)#6 (11) {
    [%u|b%"name"]=>
    %unicode|string%(3) "bar"
    [%u|b%"orgname"]=>
    %unicode|string%(3) "bar"
    [%u|b%"table"]=>
    %unicode|string%(13) "test_affected"
    [%u|b%"orgtable"]=>
    %unicode|string%(13) "test_affected"
    [%u|b%"def"]=>
    %unicode|string%(0) ""
    [%u|b%"max_length"]=>
    int(0)
    [%u|b%"length"]=>
    int(%d)
    [%u|b%"charsetnr"]=>
    int(%d)
    [%u|b%"flags"]=>
    int(0)
    [%u|b%"type"]=>
    int(253)
    [%u|b%"decimals"]=>
    int(0)
  }
}

=== fetch_field_direct ===
object(stdClass)#6 (11) {
  [%u|b%"name"]=>
  %unicode|string%(3) "foo"
  [%u|b%"orgname"]=>
  %unicode|string%(3) "foo"
  [%u|b%"table"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"orgtable"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(%d)
  [%u|b%"flags"]=>
  int(32768)
  [%u|b%"type"]=>
  int(3)
  [%u|b%"decimals"]=>
  int(0)
}
object(stdClass)#6 (11) {
  [%u|b%"name"]=>
  %unicode|string%(3) "bar"
  [%u|b%"orgname"]=>
  %unicode|string%(3) "bar"
  [%u|b%"table"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"orgtable"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(%d)
  [%u|b%"flags"]=>
  int(0)
  [%u|b%"type"]=>
  int(253)
  [%u|b%"decimals"]=>
  int(0)
}

=== fetch_field ===
object(stdClass)#6 (11) {
  [%u|b%"name"]=>
  %unicode|string%(3) "foo"
  [%u|b%"orgname"]=>
  %unicode|string%(3) "foo"
  [%u|b%"table"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"orgtable"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(%d)
  [%u|b%"flags"]=>
  int(32768)
  [%u|b%"type"]=>
  int(3)
  [%u|b%"decimals"]=>
  int(0)
}
object(stdClass)#5 (11) {
  [%u|b%"name"]=>
  %unicode|string%(3) "bar"
  [%u|b%"orgname"]=>
  %unicode|string%(3) "bar"
  [%u|b%"table"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"orgtable"]=>
  %unicode|string%(13) "test_affected"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(%d)
  [%u|b%"flags"]=>
  int(0)
  [%u|b%"type"]=>
  int(253)
  [%u|b%"decimals"]=>
  int(0)
}
done!