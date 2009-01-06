--TEST--
mysqli_fetch_field()
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
	if (!is_null($tmp = @mysqli_fetch_field()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_fetch_field($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	while ($tmp = mysqli_fetch_field($res))
		var_dump($tmp);
	var_dump($tmp);

	mysqli_free_result($res);

	// Read http://bugs.php.net/bug.php?id=42344 on defaults!
	if (NULL !== ($tmp = mysqli_fetch_field($res)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "CREATE TABLE test(id INT NOT NULL DEFAULT 1)"))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "INSERT INTO test(id) VALUES (2)"))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, "SELECT id as _default_test FROM test")) {
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	var_dump(mysqli_fetch_assoc($res));
	var_dump(mysqli_fetch_field($res));
	mysqli_free_result($res);

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
  int(254)
  [u"decimals"]=>
  int(0)
}
bool(false)

Warning: mysqli_fetch_field(): Couldn't fetch mysqli_result in %s on line %d
array(1) {
  [u"_default_test"]=>
  unicode(1) "2"
}
object(stdClass)#%d (11) {
  [u"name"]=>
  unicode(13) "_default_test"
  [u"orgname"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "test"
  [u"orgtable"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(1)
  [u"length"]=>
  int(11)
  [u"charsetnr"]=>
  int(63)
  [u"flags"]=>
  int(32769)
  [u"type"]=>
  int(3)
  [u"decimals"]=>
  int(0)
}
done!
