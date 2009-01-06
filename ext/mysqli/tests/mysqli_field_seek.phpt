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

	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_field_seek()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_field_seek($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1", MYSQLI_USE_RESULT)) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	var_dump(mysqli_field_seek($res, -1));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_field_seek($res, 0));
	var_dump(mysqli_fetch_field($res));
	var_dump(mysqli_field_seek($res, 1));
	var_dump(mysqli_fetch_field($res));
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
--EXPECTF--
Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(2) "id"
  [u"orgname"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "test"
  [u"orgtable"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(%d)
  [u"length"]=>
  int(%d)
  [u"charsetnr"]=>
  int(%d)
  [u"flags"]=>
  int(%d)
  [u"type"]=>
  int(%d)
  [u"decimals"]=>
  int(0)
}
bool(true)
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(2) "id"
  [u"orgname"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "test"
  [u"orgtable"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(%d)
  [u"length"]=>
  int(%d)
  [u"charsetnr"]=>
  int(%d)
  [u"flags"]=>
  int(%d)
  [u"type"]=>
  int(%d)
  [u"decimals"]=>
  int(0)
}
bool(true)
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(5) "label"
  [u"orgname"]=>
  unicode(5) "label"
  [u"table"]=>
  unicode(4) "test"
  [u"orgtable"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(%d)
  [u"length"]=>
  int(%d)
  [u"charsetnr"]=>
  int(%d)
  [u"flags"]=>
  int(%d)
  [u"type"]=>
  int(%d)
  [u"decimals"]=>
  int(0)
}
int(2)

Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
bool(false)

Warning: mysqli_field_seek(): Invalid field offset in %s on line %d
bool(false)
bool(true)
object(stdClass)#3 (11) {
  [u"name"]=>
  unicode(5) "_null"
  [u"orgname"]=>
  unicode(0) ""
  [u"table"]=>
  unicode(0) ""
  [u"orgtable"]=>
  unicode(0) ""
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(0)
  [u"length"]=>
  int(0)
  [u"charsetnr"]=>
  int(63)
  [u"flags"]=>
  int(32896)
  [u"type"]=>
  int(6)
  [u"decimals"]=>
  int(0)
}

Warning: mysqli_field_seek(): Couldn't fetch mysqli_result in %s on line %d
NULL
done!
