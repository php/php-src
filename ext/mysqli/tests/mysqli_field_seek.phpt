--TEST--
mysqli_field_seek()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	function mysqli_field_seek_flags($flags) {

		$ret = '';

		if ($flags & MYSQLI_NOT_NULL_FLAG)
			$ret .= 'MYSQLI_NOT_NULL_FLAG ';

		if ($flags & MYSQLI_PRI_KEY_FLAG)
			$ret .= 'MYSQLI_PRI_KEY_FLAG ';

		if ($flags & MYSQLI_UNIQUE_KEY_FLAG)
			$ret .= 'MYSQLI_UNIQUE_KEY_FLAG ';

		if ($flags & MYSQLI_MULTIPLE_KEY_FLAG)
			$ret .= 'MYSQLI_MULTIPLE_KEY_FLAG ';

		if ($flags & MYSQLI_BLOB_FLAG)
			$ret .= 'MYSQLI_BLOB_FLAG ';

		if ($flags & MYSQLI_UNSIGNED_FLAG)
			$ret .= 'MYSQLI_UNSIGNED_FLAG ';

		if ($flags & MYSQLI_ZEROFILL_FLAG)
			$ret .= 'MYSQLI_ZEROFILL_FLAG ';

		if ($flags & MYSQLI_AUTO_INCREMENT_FLAG)
			$ret .= 'MYSQLI_AUTO_INCREMENT_FLAG ';

		if ($flags & MYSQLI_TIMESTAMP_FLAG)
			$ret .= 'MYSQLI_TIMESTAMP_FLAG ';

		if ($flags & MYSQLI_SET_FLAG)
			$ret .= 'MYSQLI_SET_FLAG ';

		if ($flags & MYSQLI_NUM_FLAG)
			$ret .= 'MYSQLI_NUM_FLAG ';

		if ($flags & MYSQLI_PART_KEY_FLAG)
			$ret .= 'MYSQLI_PART_KEY_FLAG ';

		if ($flags & MYSQLI_GROUP_FLAG)
			$ret .= 'MYSQLI_GROUP_FLAG ';

		return $ret;
	}

	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_field_seek()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_field_seek($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	// Make sure that client, connection and result charsets are all the
	// same. Not sure whether this is strictly necessary.
	if (!mysqli_set_charset($link, 'utf8'))
		printf("[%d] %s\n", mysqli_errno($link), mysqli_errno($link));

	$charsetInfo = mysqli_get_charset($link);

	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1", MYSQLI_USE_RESULT)) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	var_dump(mysqli_field_seek($res, -1));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_field_seek($res, 0));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_field_seek($res, 1));

	$field = mysqli_fetch_field($res);
	var_dump($field);
	/* label column, result set charset */
	if ($field->charsetnr != $charsetInfo->number) {
		printf("[004] Expecting charset %s/%d got %d\n",
			$charsetInfo->charset, $charsetInfo->number, $field->charsetnr);
	}
	if ($field->length != $charsetInfo->max_length) {
		printf("[005] Expecting length %d got %d\n",
			$charsetInfo->max_length, $field->max_length);
	}

	var_dump(mysqli_field_tell($res));
	var_dump(mysqli_field_seek($res, 2));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_field_seek($res, PHP_INT_MAX + 1));

	if (!is_null($tmp = @mysqli_field_seek($res, 0, "too many")))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_free_result($res);

	if (!$res = mysqli_query($link, "SELECT NULL as _null", MYSQLI_STORE_RESULT)) {
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	var_dump(mysqli_field_seek($res, 0));
	var_dump(mysqli_fetch_field($res));

	mysqli_free_result($res);

	var_dump(mysqli_field_seek($res, 0));

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
object(stdClass)#%d (13) {
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
  [%u|b%"db"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"catalog"]=>
  %unicode|string%(%d) "%s"
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
bool(true)
object(stdClass)#%d (13) {
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
  [%u|b%"db"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"catalog"]=>
  %unicode|string%(%d) "%s"
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
bool(true)
object(stdClass)#%d (13) {
  [%u|b%"name"]=>
  %unicode|string%(5) "label"
  [%u|b%"orgname"]=>
  %unicode|string%(5) "label"
  [%u|b%"table"]=>
  %unicode|string%(4) "test"
  [%u|b%"orgtable"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"db"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"catalog"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"max_length"]=>
  int(%d)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(%d)
  [%u|b%"flags"]=>
  int(0)
  [%u|b%"type"]=>
  int(254)
  [%u|b%"decimals"]=>
  int(0)
}
int(2)

Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
bool(false)

Warning: mysqli_field_seek() expects parameter 2 to be integer, float given in %s on line %d
NULL
bool(true)
object(stdClass)#%d (13) {
  [%u|b%"name"]=>
  %unicode|string%(5) "_null"
  [%u|b%"orgname"]=>
  %unicode|string%(0) ""
  [%u|b%"table"]=>
  %unicode|string%(0) ""
  [%u|b%"orgtable"]=>
  %unicode|string%(0) ""
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"db"]=>
  %unicode|string%(0) ""
  [%u|b%"catalog"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(0)
  [%u|b%"charsetnr"]=>
  int(63)
  [%u|b%"flags"]=>
  int(32896)
  [%u|b%"type"]=>
  int(6)
  [%u|b%"decimals"]=>
  int(0)
}

Warning: mysqli_field_seek(): Couldn't fetch mysqli_result in %s on line %d
NULL
done!
