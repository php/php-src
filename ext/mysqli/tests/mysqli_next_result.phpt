--TEST--
mysqli_next_result()
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

	if (!is_null($tmp = @mysqli_next_result()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_next_result($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (false !== ($tmp = mysqli_next_result($link)))
		printf("[003] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	$res = mysqli_query($link, "SELECT 1 AS res");
	if (false !== ($tmp = mysqli_next_result($link)))
		printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_free_result($res);

	function func_test_mysqli_next_result($link, $query, $offset, $num_results) {

		if (!mysqli_multi_query($link, $query))
			printf("[%03d] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));

		$i = 0;
		do {
			if ($res = mysqli_store_result($link)) {
				mysqli_free_result($res);
				$i++;
			}
		} while (true === mysqli_next_result($link));

		if ($i !== $num_results) {
			printf("[%03d] Expecting %d result(s), got %d result(s)\n", $offset + 2, $num_results, $i);
		}

		if (mysqli_more_results($link))
			printf("[%03d] mysqli_more_results() indicates more results than expected\n", $offset + 3);

		if (!($res = mysqli_query($link, "SELECT 1 AS b"))) {
			printf("[%03d] [%d] %s\n", $offset + 4, mysqli_errno($link), mysqli_error($link));
		} else {
			mysqli_free_result($res);
		}

	}

	func_test_mysqli_next_result($link, "SELECT 1 AS a; SELECT 1 AS a, 2 AS b; SELECT id FROM test ORDER BY id LIMIT 3", 5, 3);
	func_test_mysqli_next_result($link, "SELECT 1 AS a; INSERT INTO test(id, label) VALUES (100, 'y'); SELECT 1 AS a, 2 AS b", 8, 2);
	func_test_mysqli_next_result($link, "DELETE FROM test WHERE id >= 100; SELECT 1 AS a; ", 11, 1);

	mysqli_close($link);

	var_dump(mysqli_next_result($link));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--

Warning: mysqli_next_result(): Couldn't fetch mysqli in %s on line %d
bool(false)
done!
