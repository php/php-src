--TEST--
mysqli_fetch_row()
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

	if (!is_null($tmp = @mysqli_fetch_row()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_fetch_row($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id, label, id AS _id FROM test ORDER BY id LIMIT 1")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	print "[004]\n";
	var_dump(mysqli_fetch_row($res));

	print "[005]\n";
	var_dump(mysqli_fetch_row($res));

	mysqli_free_result($res);

	var_dump(mysqli_fetch_row($res));

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
[004]
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "1"
}
[005]
NULL

Warning: mysqli_fetch_row(): Couldn't fetch mysqli_result in %s on line %d
NULL
done!
