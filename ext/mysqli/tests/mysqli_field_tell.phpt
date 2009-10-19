--TEST--
mysqli_field_tell()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_field_tell()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_field_tell($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id FROM test ORDER BY id LIMIT 1", MYSQLI_USE_RESULT)) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	var_dump(mysqli_field_tell($res));
	var_dump(mysqli_field_seek(1));
	var_dump(mysqli_field_tell($res));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_field_tell($res));

	if (!is_null($tmp = @mysqli_field_tell($res, 'too many arguments')))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);


	var_dump(mysqli_field_seek($res, 2));
	var_dump(mysqli_field_tell($res));

	var_dump(mysqli_field_seek($res, -1));
	var_dump(mysqli_field_tell($res));

	var_dump(mysqli_field_seek($res, 0));
	var_dump(mysqli_field_tell($res));



	mysqli_free_result($res);

	var_dump(mysqli_field_tell($res));

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
int(0)

Warning: mysqli_field_seek() expects exactly 2 parameters, 1 given in %s on line %d
NULL
int(0)
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(2) "id"
  [%u|b%"orgname"]=>
  %unicode|string%(2) "id"
  [%u|b%"table"]=>
  %unicode|string%(4) "test"
  [%u|b%"orgtable"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(11)
  [%u|b%"charsetnr"]=>
  int(63)
  [%u|b%"flags"]=>
  int(49155)
  [%u|b%"type"]=>
  int(3)
  [%u|b%"decimals"]=>
  int(0)
}
bool(false)
int(1)

Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
int(1)

Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
int(1)
bool(true)
int(0)

Warning: mysqli_field_tell(): Couldn't fetch mysqli_result in %s on line %d
NULL
done!