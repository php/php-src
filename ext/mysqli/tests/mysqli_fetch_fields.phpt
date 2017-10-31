--TEST--
mysqli_fetch_fields()
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

	// Note: no SQL type tests, internally the same function gets used as for mysqli_fetch_array() which does a lot of SQL type test
	if (!is_null($tmp = @mysqli_fetch_fields()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_fetch_fields($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	// Make sure that client, connection and result charsets are all the
	// same. Not sure whether this is strictly necessary.
	if (!mysqli_set_charset($link, 'utf8'))
		printf("[%d] %s\n", mysqli_errno($link), mysqli_errno($link));

	$charsetInfo = mysqli_get_charset($link);

	if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	$fields = mysqli_fetch_fields($res);
	foreach ($fields as $k => $field) {
		var_dump($field);
		switch ($k) {
			case 1:
				/* label column, result set charset */
				if ($field->charsetnr != $charsetInfo->number) {
					printf("[004] Expecting charset %s/%d got %d\n",
						$charsetInfo->charset,
						$charsetInfo->number, $field->charsetnr);
				}
				if ($field->length != $charsetInfo->max_length) {
					printf("[005] Expecting length %d got %d\n",
						$charsetInfo->max_length,
						$field->max_length);
				}
				break;
		}
	}

	mysqli_free_result($res);

	if (false !== ($tmp = mysqli_fetch_fields($res)))
		printf("[006] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
object(stdClass)#%d (13) {
  ["name"]=>
  string(2) "ID"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "TEST"
  ["orgtable"]=>
  string(%d) "%s"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(1)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(49155)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
object(stdClass)#%d (13) {
  ["name"]=>
  string(5) "label"
  ["orgname"]=>
  string(5) "label"
  ["table"]=>
  string(4) "TEST"
  ["orgtable"]=>
  string(%d) "%s"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(1)
  ["length"]=>
  int(%d)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(0)
  ["type"]=>
  int(254)
  ["decimals"]=>
  int(0)
}

Warning: mysqli_fetch_fields(): Couldn't fetch mysqli_result in %s on line %d
done!
