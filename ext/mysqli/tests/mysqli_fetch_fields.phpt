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
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	// Note: no SQL type tests, internally the same function gets used as for mysqli_fetch_array() which does a lot of SQL type test
	if (!is_null($tmp = @mysqli_fetch_fields()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_fetch_fields($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	$fields = mysqli_fetch_fields($res);
	foreach ($fields as $k => $field)
		var_dump($field);

	mysqli_free_result($res);

	if (NULL !== ($tmp = mysqli_fetch_fields($res)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(2) "ID"
  [u"orgname"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "TEST"
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
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(5) "label"
  [u"orgname"]=>
  unicode(5) "label"
  [u"table"]=>
  unicode(4) "TEST"
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
  int(0)
  [u"type"]=>
  int(%d)
  [u"decimals"]=>
  int(0)
}

Warning: mysqli_fetch_fields(): Couldn't fetch mysqli_result in %s on line %d
done!
