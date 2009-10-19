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

	if (!($stmt = mysqli_stmt_init($link)) ||
		!mysqli_stmt_prepare($stmt, "SELECT id, label, id + 1 as _id,  concat(label, '_') ___label FROM test ORDER BY id ASC LIMIT 3") ||
		!mysqli_stmt_execute($stmt))
		printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
		printf("[007] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	if (!is_object($res_meta = mysqli_stmt_result_metadata($stmt)) ||
		'mysqli_result' != get_class($res_meta)) {
		printf("[008] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	$fields = array();
	while ($info = $res->fetch_field()) {
		var_dump($info);
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
object(stdClass)#%d (11) {
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
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"length"]=>
  int(1)
  [%u|b%"charsetnr"]=>
  int(8)
  [%u|b%"flags"]=>
  int(0)
  [%u|b%"type"]=>
  int(254)
  [%u|b%"decimals"]=>
  int(0)
}
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(3) "_id"
  [%u|b%"orgname"]=>
  %unicode|string%(0) ""
  [%u|b%"table"]=>
  %unicode|string%(0) ""
  [%u|b%"orgtable"]=>
  %unicode|string%(0) ""
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"length"]=>
  int(%d)
  [%u|b%"charsetnr"]=>
  int(63)
  [%u|b%"flags"]=>
  int(32897)
  [%u|b%"type"]=>
  int(8)
  [%u|b%"decimals"]=>
  int(0)
}
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(8) "___label"
  [%u|b%"orgname"]=>
  %unicode|string%(0) ""
  [%u|b%"table"]=>
  %unicode|string%(0) ""
  [%u|b%"orgtable"]=>
  %unicode|string%(0) ""
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(2)
  [%u|b%"length"]=>
  int(2)
  [%u|b%"charsetnr"]=>
  int(8)
  [%u|b%"flags"]=>
  int(0)
  [%u|b%"type"]=>
  int(253)
  [%u|b%"decimals"]=>
  int(31)
}
done!