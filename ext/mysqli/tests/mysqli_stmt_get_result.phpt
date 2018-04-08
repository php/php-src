--TEST--
mysqli_stmt_get_result()
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
	/*
	NOTE: no datatype tests here! This is done by
	mysqli_stmt_bind_result.phpt already. Restrict
	this test case to the basics.
	*/
	require_once("connect.inc");

	$tmp	= NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_stmt_get_result()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_get_result($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	// stmt object status test
	if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
		printf("[004] Expecting false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
		printf("[005] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	// FIXME - different versions return different values ?!
	if ((NULL !== ($tmp = mysqli_stmt_get_result($stmt))) && (false !== $tmp))
		printf("[006] Expecting NULL or boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!mysqli_stmt_execute($stmt))
		printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_store_result($stmt))
		printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (is_object($tmp = mysqli_stmt_store_result($stmt)))
		printf("[009] non-object, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	mysqli_stmt_close($stmt);

	if (!$stmt = mysqli_stmt_init($link))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	// stmt object status test
	if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
		printf("[011] Expecting false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
		printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	// FIXME - different versions return different values ?!
	if ((NULL !== ($tmp = mysqli_stmt_get_result($stmt))) && (false !== $tmp))
		printf("[013] Expecting NULL or boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!mysqli_stmt_execute($stmt))
		printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($tmp = mysqli_stmt_get_result($stmt)))
		printf("[016] NULL, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	mysqli_free_result($tmp);
	mysqli_stmt_close($stmt);

	if (!$stmt = mysqli_stmt_init($link))
		printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	// stmt object status test
	if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
		printf("[018] Expecting false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
		printf("[019] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
		printf("[020] Expecting false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!mysqli_stmt_execute($stmt))
		printf("[023] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($tmp = mysqli_stmt_get_result($stmt)))
		printf("[024] Expecting object, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
		printf("[025] false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (true !== ($tmp = mysqli_stmt_bind_result($stmt, $id, $label))) {
		printf("[026] [%d] [%s]\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
		printf("[027] [%d] [%s]\n", mysqli_errno($link), mysqli_error($link));
		printf("[028] Expecting boolean/true, got %s/%s\n", gettype($tmp), var_export($tmp, 1));
	}

	if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
		printf("[029] false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	mysqli_stmt_close($stmt);

	if (!$stmt = mysqli_stmt_init($link))
		printf("[032] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id LIMIT 2"))
		printf("[033] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[034] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = NULL;
	$label = NULL;
	if (true !== ($tmp = mysqli_stmt_bind_result($stmt, $id, $label)))
		printf("[035] Expecting boolean/true, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	if (!is_object($tmp = $result = mysqli_stmt_get_result($stmt)))
		printf("[036] Expecting array, got %s/%s, [%d] %s\n",
			gettype($tmp), var_export($tmp, 1),
			mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
		printf("[037] Expecting boolean/false, got %s/%s, [%d] %s\n",
			gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	printf("[038] [%d] [%s]\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	printf("[039] [%d] [%s]\n", mysqli_errno($link), mysqli_error($link));
	while ($row = mysqli_fetch_assoc($result)) {
		var_dump($row);
	}
	mysqli_free_result($result);

	if (!mysqli_kill($link, mysqli_thread_id($link)))
		printf("[040] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (false !== ($tmp = mysqli_stmt_get_result($stmt)))
		printf("[041] Expecting false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	mysqli_stmt_close($stmt);

	if (false !== ($tmp = mysqli_stmt_fetch($stmt)))
		printf("[042] Expecting false, got %s/%s\n", gettype($tmp), var_export($tmp, 1));

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_fetch(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_fetch(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_get_result(): invalid object or resource mysqli_stmt
 in %s on line %d
[038] [2014] [Commands out of sync; you can't run this command now]
[039] [0] []
array(2) {
  ["id"]=>
  int(1)
  ["label"]=>
  %s(1) "a"
}
array(2) {
  ["id"]=>
  int(2)
  ["label"]=>
  %s(1) "b"
}

Warning: mysqli_stmt_fetch(): Couldn't fetch mysqli_stmt in %s on line %d
done!
