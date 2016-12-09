--TEST--
mysqli_stmt_get_result() - meta data, field info
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_stmt_get_result'))
	die('skip mysqli_stmt_get_result not available');
?>
--FILE--
<?php
	require('table.inc');

	// Make sure that client, connection and result charsets are all the
	// same. Not sure whether this is strictly necessary.
	if (!mysqli_set_charset($link, 'utf8'))
		printf("[%d] %s\n", mysqli_errno($link), mysqli_errno($link));

	$charsetInfo = mysqli_get_charset($link);

	if (!($stmt = mysqli_stmt_init($link)) ||
		!mysqli_stmt_prepare($stmt, "SELECT id, label, id + 1 as _id,  concat(label, '_') ___label FROM test ORDER BY id ASC LIMIT 3") ||
		!mysqli_stmt_execute($stmt))
		printf("[001] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
		printf("[002] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	if (!is_object($res_meta = mysqli_stmt_result_metadata($stmt)) ||
		'mysqli_result' != get_class($res_meta)) {
		printf("[003] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	$i = 0;
	while ($field = $res->fetch_field()) {
		var_dump($field);
		$i++;
		if (2 == $i) {
			/*
			Label column, result set charset.
			All of the following columns are "too hot" - too server dependent
			*/
			if ($field->charsetnr != $charsetInfo->number) {
				printf("[004] Expecting charset %s/%d got %d\n",
					$charsetInfo->charset,
					$charsetInfo->number, $field->charsetnr);
			}
			if ($field->length != $charsetInfo->max_length) {
				printf("[005] Expecting length %d got %d\n",
					$charsetInfo->max_length, $field->max_length);
			}
		}
	}

	mysqli_stmt_close($stmt);
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
  string(2) "id"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "test"
  ["orgtable"]=>
  string(4) "test"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(0)
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
  string(4) "test"
  ["orgtable"]=>
  string(4) "test"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(%d)
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
object(stdClass)#%d (13) {
  ["name"]=>
  string(3) "_id"
  ["orgname"]=>
  string(0) ""
  ["table"]=>
  string(0) ""
  ["orgtable"]=>
  string(0) ""
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(0) ""
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(0)
  ["length"]=>
  int(%d)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(32897)
  ["type"]=>
  int(8)
  ["decimals"]=>
  int(0)
}
object(stdClass)#%d (13) {
  ["name"]=>
  string(8) "___label"
  ["orgname"]=>
  string(0) ""
  ["table"]=>
  string(0) ""
  ["orgtable"]=>
  string(0) ""
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(0) ""
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(%d)
  ["length"]=>
  int(%d)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(0)
  ["type"]=>
  int(253)
  ["decimals"]=>
  int(31)
}
done!
