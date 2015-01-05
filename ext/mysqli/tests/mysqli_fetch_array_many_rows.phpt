--TEST--
mysqli_fetch_array()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require("table.inc");

	// do as much as we can do in 5 seconds
	$start = microtime(true);
	for ($id = 100, $start = microtime(true); (microtime(true) - $start) < 5; $id++) {
		if (!mysqli_query($link, $sql = sprintf("INSERT INTO test(id, label) VALUES (%d, '%s')",
			$id, mysqli_real_escape_string($link, chr(65 + ($id % 26)))))) {
				printf("[001] %s failed: [%d] %s\n", $sql, mysqli_errno($link), mysqli_error($link));
				break;
		}
	}

	if (!$res = mysqli_query($link, 'SELECT id, label FROM test')) {
		printf("[002] SELECT failed: [%d] %s\n", mysqli_errno($link), mysqli_errno($link));
	}

	while ($row = mysqli_fetch_array($res)) {
		// overwrite results and check if the cache magic works
		$row['label'] = NULL;
	}
	mysqli_free_result($res);

	if (!$res = mysqli_query($link, 'SELECT id, label FROM test')) {
		printf("[003] SELECT failed: [%d] %s\n", mysqli_errno($link), mysqli_errno($link));
	}

	$i = 0;
	$results = array();
	while ($row = mysqli_fetch_array($res, MYSQLI_BOTH)) {

		// create copies and destroy later
		$results[$i++] = &$row;
		if ($i % 999) {
			$results = array();
		}

		if ($row[0] < 0 || $row[0] > $id) {
			printf("[004] Unexpected result row[0] = '%s' (range 0...%d), [%d] %s\n",
				$row[0], $id, mysqli_errno($link), mysqli_error($link));
			break;
		}
		if ($row[0] !== $row['id']) {
			printf("[005] Unexpected result row[0] = '%s', row[id] = '%s', [%d] %s\n",
				$row[0], $row[id], mysqli_errno($link), mysqli_error($link));
			break;
		}

		$len = strlen($row[1]);
		if (!is_string($row[1]) || $len == 0 || $len > 1) {
			printf("[006] Unexpected result row[1] = '%s', [%d] %s\n",
				$row[1], mysqli_errno($link), mysqli_error($link));
			break;
		}
		if ($row[1] !== $row['label']) {
			printf("[007] Unexpected result row[1] = '%s', row[label] = '%s', [%d] %s\n",
				$row[1], $row['label'], mysqli_errno($link), mysqli_error($link));
			break;
		}

	}
	mysqli_free_result($res);

	if (!$res = mysqli_query($link, 'SELECT id, label FROM test')) {
		printf("[008] SELECT failed: [%d] %s\n", mysqli_errno($link), mysqli_errno($link));
	}

	while ($row = mysqli_fetch_array($res, MYSQLI_ASSOC)) {
		// overwrite results and check if the cache magic works
		$row['label'] = NULL;
	}
	mysqli_free_result($res);

	if (!$res = mysqli_query($link, 'SELECT count(*) AS num FROM test')) {
		printf("[009] SELECT failed: [%d] %s\n", mysqli_errno($link), mysqli_errno($link));
	}
	$row = mysqli_fetch_assoc($res);
	$num = $row['num'];
	mysqli_free_result($res);

	if (!$res = mysqli_query($link, 'SELECT id, label FROM test')) {
		printf("[010] SELECT failed: [%d] %s\n", mysqli_errno($link), mysqli_errno($link));
	}

	$i = 0;
	while ($row = mysqli_fetch_array($res, MYSQLI_NUM)) {
		// overwrite results and check if the cache magic works
		$row[0] = NULL;
		$i++;
	}
	mysqli_free_result($res);

	if ($i != $num)
		printf("[011] Expecting %d results, got %d results, [%d] %s\n",
			$num, $i, mysqli_errno($link), mysqli_error($link));

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!