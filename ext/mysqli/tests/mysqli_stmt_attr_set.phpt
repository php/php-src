--TEST--
mysqli_stmt_attr_set() - mysqlnd does not check for invalid codes
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

	if (!is_null($tmp = @mysqli_stmt_attr_set()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_attr_set($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_attr_set($link, $link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_attr_set($link, $link, $link)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	$valid_attr = array(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH);
	if ((mysqli_get_client_version() > 50003) || $IS_MYSQLND) {
		$valid_attr[] = MYSQLI_STMT_ATTR_CURSOR_TYPE;
		$valid_attr[] =	MYSQLI_CURSOR_TYPE_NO_CURSOR;
		$valid_attr[] =	MYSQLI_CURSOR_TYPE_READ_ONLY;
		$valid_attr[] =	MYSQLI_CURSOR_TYPE_FOR_UPDATE;
		$valid_attr[] =	MYSQLI_CURSOR_TYPE_SCROLLABLE;
	}

	if ((mysqli_get_client_version() > 50007) || $IS_MYSQLND)
		$valid_attr[] = MYSQLI_STMT_ATTR_PREFETCH_ROWS;


	$stmt = mysqli_stmt_init($link);
	if (false !== ($tmp = @mysqli_stmt_attr_set($stmt, 0, 0)))
		printf("[005] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	$stmt->prepare("SELECT * FROM test");

	mt_srand(microtime(true));

	for ($i = -100; $i < 1000; $i++) {
		if (in_array($i, $valid_attr))
			continue;
		$invalid_attr = $i;
		if (false !== ($tmp = @mysqli_stmt_attr_set($stmt, $invalid_attr, 0))) {
			printf("[006a] Expecting boolean/false for attribute %d, got %s/%s\n", $invalid_attr, gettype($tmp), $tmp);
		}
	}

	for ($i = 0; $i < 2; $i++) {
		do {
			$invalid_attr = mt_rand(-1 * (min(4294967296, PHP_INT_MAX) + 1), min(4294967296, PHP_INT_MAX));
		} while (in_array($invalid_attr, $valid_attr));
		if (false !== ($tmp = @mysqli_stmt_attr_set($stmt, $invalid_attr, 0))) {
			printf("[006b] Expecting boolean/false for attribute %d, got %s/%s\n", $invalid_attr, gettype($tmp), $tmp);
		}
	}
	$stmt->close();

	//
	// MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH
	//


	// expecting max_length not to be set and be 0 in all cases
	$stmt = mysqli_stmt_init($link);
	$stmt->prepare("SELECT label FROM test");
	$stmt->execute();
	$stmt->store_result();
	$res = $stmt->result_metadata();
	$fields = $res->fetch_fields();
	$max_lengths = array();
	foreach ($fields as $k => $meta) {
		$max_lengths[$meta->name] = $meta->max_length;
		if ($meta->max_length !== 0)
			printf("[007] max_length should be not set (= 0), got %s for field %s\n", $meta->max_length, $meta->name);
	}
	$res->close();
	$stmt->close();

	// expecting max_length to _be_ set
	$stmt = mysqli_stmt_init($link);
	$stmt->prepare("SELECT label FROM test");
	$stmt->attr_set(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, 1);
	$res = $stmt->attr_get(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH);
	if ($res !== 1)
		printf("[007.1] max_length should be 1, got %s\n", $res);
	$stmt->execute();
	$stmt->store_result();
	$res = $stmt->result_metadata();
	$fields = $res->fetch_fields();
	$max_lengths = array();
	foreach ($fields as $k => $meta) {
		$max_lengths[$meta->name] = $meta->max_length;
		if ($meta->max_length === 0)
			printf("[008] max_length should be set (!= 0), got %s for field %s\n", $meta->max_length, $meta->name);
	}
	$res->close();
	$stmt->close();

	// expecting max_length not to be set
	$stmt = mysqli_stmt_init($link);
	$stmt->prepare("SELECT label FROM test");
	$stmt->attr_set(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, 0);
	$res = $stmt->attr_get(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH);
	if ($res !== 0)
		printf("[008.1] max_length should be 0, got %s\n", $res);
	$stmt->execute();
	$stmt->store_result();
	$res = $stmt->result_metadata();
	$fields = $res->fetch_fields();
	$max_lengths = array();
	foreach ($fields as $k => $meta) {
		$max_lengths[$meta->name] = $meta->max_length;
		if ($meta->max_length !== 0)
			printf("[009] max_length should not be set (= 0), got %s for field %s\n", $meta->max_length, $meta->name);
	}
	$res->close();
	$stmt->close();

	//
	// Cursors
	//

	if (mysqli_get_client_version() > 50003) {

		$cursor_types = array(
			MYSQLI_CURSOR_TYPE_NO_CURSOR,
			MYSQLI_CURSOR_TYPE_READ_ONLY,
			MYSQLI_CURSOR_TYPE_FOR_UPDATE,
			MYSQLI_CURSOR_TYPE_SCROLLABLE
		);
		do {
			$invalid_cursor_type = mt_rand(-1000, 1000);
		} while (in_array($invalid_cursor_type, $cursor_types));

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT id, label FROM test");

		if (false !== ($tmp = @$stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, $invalid_cursor_type)))
			printf("[010] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

		if (false !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_FOR_UPDATE)))
			printf("[011] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

		if (false !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_SCROLLABLE)))
			printf("[012] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_NO_CURSOR)))
			printf("[013] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY)))
			printf("[014] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

		$stmt->close();

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT id, label FROM test");
		$stmt->execute();
		$id = $label = NULL;
		$stmt->bind_result($id, $label);
		$results = array();
		while ($stmt->fetch())
			$results[$id] = $label;
		$stmt->close();
		if (empty($results))
			printf("[015] Results should not be empty, subsequent tests will probably fail!\n");

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT id, label FROM test");
		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_NO_CURSOR)))
			printf("[016] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
		$stmt->execute();
		$id = $label = NULL;
		$stmt->bind_result($id, $label);
		$results2 = array();
		while ($stmt->fetch())
			$results2[$id] = $label;
		$stmt->close();
		if ($results != $results2) {
			printf("[017] Results should not differ. Dumping both result sets.\n");
			var_dump($results);
			var_dump($results2);
		}

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT id, label FROM test");
		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY)))
			printf("[018] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
		$stmt->execute();
		$id = $label = NULL;
		$stmt->bind_result($id, $label);
		$results2 = array();
		while ($stmt->fetch())
			$results2[$id] = $label;
		$stmt->close();
		if ($results != $results2) {
			printf("[019] Results should not differ. Dumping both result sets.\n");
			var_dump($results);
			var_dump($results2);
		}

	}


	//
	// MYSQLI_STMT_ATTR_PREFETCH_ROWS
	//

	if (mysqli_get_client_version() > 50007) {

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT id, label FROM test");
		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, 1)))
			printf("[020] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
		$stmt->execute();
		$id = $label = NULL;
		$stmt->bind_result($id, $label);
		$results = array();
		while ($stmt->fetch())
			$results[$id] = $label;
		$stmt->close();
		if (empty($results))
			printf("[021] Results should not be empty, subsequent tests will probably fail!\n");

		/* prefetch is not supported
		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT label FROM test");
		if (false !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, -1)))
			printf("[022] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
		$stmt->close();

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT label FROM test");
		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, PHP_INT_MAX)))
				printf("[023] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
		$stmt->close();

		$stmt = mysqli_stmt_init($link);
		$stmt->prepare("SELECT id, label FROM test");
		if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, 2)))
			printf("[024] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
		$stmt->execute();
		$id = $label = NULL;
		$stmt->bind_result($id, $label);
		$results2 = array();
		while ($stmt->fetch())
			$results2[$id] = $label;
		$stmt->close();
		if ($results != $results2) {
			printf("[025] Results should not differ. Dumping both result sets.\n");
			var_dump($results);
			var_dump($results2);
		}
		*/

	}

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
done!
