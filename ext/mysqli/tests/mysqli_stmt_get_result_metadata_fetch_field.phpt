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
	include "connect.inc";
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
--EXPECTF--
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
  int(0)
  [u"length"]=>
  int(11)
  [u"charsetnr"]=>
  int(63)
  [u"flags"]=>
  int(49155)
  [u"type"]=>
  int(3)
  [u"decimals"]=>
  int(0)
}
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
  int(1)
  [u"length"]=>
  int(3)
  [u"charsetnr"]=>
  int(33)
  [u"flags"]=>
  int(0)
  [u"type"]=>
  int(254)
  [u"decimals"]=>
  int(0)
}
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(3) "_id"
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
  int(%d)
  [u"charsetnr"]=>
  int(63)
  [u"flags"]=>
  int(32897)
  [u"type"]=>
  int(8)
  [u"decimals"]=>
  int(0)
}
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(8) "___label"
  [u"orgname"]=>
  unicode(0) ""
  [u"table"]=>
  unicode(0) ""
  [u"orgtable"]=>
  unicode(0) ""
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(2)
  [u"length"]=>
  int(6)
  [u"charsetnr"]=>
  int(33)
  [u"flags"]=>
  int(0)
  [u"type"]=>
  int(253)
  [u"decimals"]=>
  int(31)
}
done!
