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
	require_once("connect.inc");

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
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(2) "ID"
  [%u|b%"orgname"]=>
  %unicode|string%(2) "id"
  [%u|b%"table"]=>
  %unicode|string%(4) "TEST"
  [%u|b%"orgtable"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
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
  %unicode|string%(4) "TEST"
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
bool(false)

Warning: mysqli_fetch_field(): Couldn't fetch mysqli_result in %s on line %d
array(1) {
  [%u|b%"_default_test"]=>
  %unicode|string%(1) "2"
}
object(stdClass)#%d (11) {
  [%u|b%"name"]=>
  %unicode|string%(13) "_default_test"
  [%u|b%"orgname"]=>
  %unicode|string%(2) "id"
  [%u|b%"table"]=>
  %unicode|string%(4) "test"
  [%u|b%"orgtable"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"length"]=>
  int(11)
  [%u|b%"charsetnr"]=>
  int(63)
  [%u|b%"flags"]=>
  int(32769)
  [%u|b%"type"]=>
  int(3)
  [%u|b%"decimals"]=>
  int(0)
}
done!