--TEST--
mysqli_warning_count()
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

	if (!is_null($tmp = @mysqli_warning_count()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_warning_count($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (NULL !== ($tmp = @mysqli_warning_count($link, "too_many")))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, "SELECT id, label FROM test"))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (0 !== ($tmp = mysqli_warning_count($link)))
		printf("[005] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "DROP TABLE IF EXISTS this_table_does_not_exist"))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (1 !== ($tmp = mysqli_warning_count($link)))
		printf("[007] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	if (false !== ($tmp = mysqli_warning_count($link)))
		printf("[010] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_warning_count(): Couldn't fetch mysqli in %s on line %d
done!
