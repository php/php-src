--TEST--
mysqli_field_count()
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

	if (!is_null($tmp = @mysqli_field_count()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_field_count($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	var_dump(mysqli_field_count($link));

	if (!$res = mysqli_query($link, "SELECT * FROM test ORDER BY id LIMIT 1")) {
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	var_dump(mysqli_field_count($link));

	mysqli_free_result($res);

	if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, 'x')"))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	var_dump($link->field_count);
	var_dump(mysqli_field_count($link));

	if (!$res = mysqli_query($link, "SELECT NULL as _null, '' AS '', 'three' AS 'drei'"))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	var_dump(mysqli_field_count($link));
	mysqli_free_result($res);

	mysqli_close($link);

	var_dump(mysqli_field_count($link));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
int(0)
int(2)
int(0)
int(0)
int(3)

Warning: mysqli_field_count(): Couldn't fetch mysqli in %s on line %d
NULL
done!
