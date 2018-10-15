--TEST--
mysqli_stmt_num_rows()
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

	if (!is_null($tmp = @mysqli_stmt_num_rows()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_num_rows($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	function func_test_mysqli_stmt_num_rows($stmt, $query, $expected, $offset) {

		if (!mysqli_stmt_prepare($stmt, $query)) {
			printf("[%03d] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_execute($stmt)) {
			printf("[%03d] [%d] %s\n", $offset + 1, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_store_result($stmt)) {
			printf("[%03d] [%d] %s\n", $offset + 2, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if ($expected !== ($tmp = mysqli_stmt_num_rows($stmt)))
			printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 3,
				gettype($expected), $expected,
				gettype($tmp), $tmp);

		mysqli_stmt_free_result($stmt);

		return true;
	}

	func_test_mysqli_stmt_num_rows($stmt, "SELECT 1 AS a", 1, 10);
	func_test_mysqli_stmt_num_rows($stmt, "SHOW VARIABLES LIKE '%nixnutz%'", 0, 20);
	// Note: for statements that return no result set mysqli_num_rows() differs from mysqli_stmt_num_rows() slightly
	// mysqli_num_rows() failed to fetch the result set and the PHP parameter check makes it return NULL
	// mysqli_stmt_numrows() has a valid resource to work on and it will return int/0 instead. No bug, but
	// slightly different behaviour... - if you really check the data types and don't rely on casting like 98% of all PHP
	// users do.
	func_test_mysqli_stmt_num_rows($stmt, "INSERT INTO test(id, label) VALUES (100, 'z')", 0, 30);

	if ($res = mysqli_query($link, 'SELECT COUNT(id) AS num FROM test')) {
		$row = mysqli_fetch_assoc($res);
		mysqli_free_result($res);
		func_test_mysqli_stmt_num_rows($stmt, "SELECT id, label FROM test", (int)$row['num'], 40);
	} else {
		printf("[050] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	print "run_tests.php don't fool me with your 'ungreedy' expression '.+?'!\n";

	if (!mysqli_stmt_prepare($stmt, 'SELECT id FROM test'))
		printf("[051] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (mysqli_stmt_execute($stmt)) {

		$i = 0;
		do {
			if (0 !== ($tmp = mysqli_stmt_num_rows($stmt)))
				printf("[53 - %03d] Expecting int/0, got %s/%s\n", $i, gettype($tmp), $tmp);
			$i++;
		} while (mysqli_stmt_fetch($stmt));

		/* NOTE to users
		Behaviour with libmysql is UNDEFINED, see http://news.php.net/php.internals/55210
		Because it is undefined it is allowed to the mysqlnd DEVELOPER to implement
		any behaviour they like, including the one checked for in this test.
		What the test does is cover an implementation detail of the mysqlnd library.
		This implementation detail may, at any time, change without prior notice.
		On the contrary, the mysqlnd way is a reasonable one and, maybe, one fine
		day, after Klingons visited earh, becomes the official one. Meanwhile do
		not rely on it.
		*/
		if ($IS_MYSQLND && (7 !== ($tmp = mysqli_stmt_num_rows($stmt))))
			printf("[54] Expecting int/7, got %s/%s\n", gettype($tmp), $tmp);

	} else {
		printf("[055] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	mysqli_stmt_close($stmt);

	if (NULL !== ($tmp = mysqli_stmt_num_rows($stmt)))
		printf("[056] Expecting NULL, got %s/%s\n");

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
run_tests.php don't fool me with your 'ungreedy' expression '.+?'!

Warning: mysqli_stmt_num_rows(): Couldn't fetch mysqli_stmt in %s on line %d
done!
