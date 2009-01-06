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
	include "connect.inc";

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
--EXPECTF--
int(0)

Warning: mysqli_field_seek() expects exactly 2 parameters, 1 given in %s on line %d
NULL
int(0)
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
