--TEST--
mysqli_set_opt() - MYSQLI_OPT_NUMERIC_AND_DATETIME_AS_UNICODE
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (version_compare(PHP_VERSION, '5.9.9', '<') == 1) {
	die('skip Needs PHP 6 and Unicode');
}

if (!stristr(mysqli_get_client_info(), "mysqlnd"))
	die("skip works only with mysqlnd");
?>
--FILE--
<?php
	include "connect.inc";
	include "table.inc";

	if (true !== ($tmp = mysqli_set_opt($link, MYSQLI_OPT_NUMERIC_AND_DATETIME_AS_UNICODE, 1)))
		printf("[001] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, 'ALTER TABLE test ADD col_date DATE,
		ADD col_time TIME,
		ADD col_timestamp TIMESTAMP,
		ADD col_datetime DATETIME'))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'UPDATE test SET col_date = NOW(),
		col_time = NOW(),
		col_timestamp = NOW(),
		col_datetime = NOW()'))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT * FROM test'))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!is_unicode($row['col_time']) || '' == $row['col_time'])
		printf("[006] Expecting unicode/any, got %s/%s\n", gettype($row['col_time']), $row['col_time']);

	if (!is_unicode($row['col_timestamp']) || '' == $row['col_timestamp'])
		printf("[007] Expecting unicode/any, got %s/%s\n", gettype($row['col_timestamp']), $row['col_timestamp']);

	if (!is_unicode($row['col_datetime']) || '' == $row['col_datetime'])
		printf("[008] Expecting unicode/any, got %s/%s\n", gettype($row['col_datetime']), $row['col_datetime']);

	if (!is_unicode($row['col_date']) || '' == $row['col_date'])
		printf("[009] Expecting unicode/any, got %s/%s\n", gettype($row['col_date']), $row['col_date']);

	mysqli_free_result($res);

	if (true !== ($tmp = mysqli_set_opt($link, MYSQLI_OPT_NUMERIC_AND_DATETIME_AS_UNICODE, 0)))
		printf("[010] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, 'SELECT * FROM test'))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (is_unicode($row['col_time']) || '' == $row['col_time'])
		printf("[013] Expecting (binary) string/any, got %s/%s\n", gettype($row['col_time']), $row['col_time']);

	if (is_unicode($row['col_timestamp']) || '' == $row['col_timestamp'])
		printf("[014] Expecting (binary) string/any, got %s/%s\n", gettype($row['col_timestamp']), $row['col_timestamp']);

	if (is_unicode($row['col_datetime']) || '' == $row['col_datetime'])
		printf("[015] Expecting (binary) string/any, got %s/%s\n", gettype($row['col_datetime']), $row['col_datetime']);

	if (is_unicode($row['col_date']) || '' == $row['col_date'])
		printf("[016] Expecting (binary) string/any, got %s/%s\n", gettype($row['col_date']), $row['col_date']);

	mysqli_free_result($res);

	if (true !== ($tmp = mysqli_set_opt($link, MYSQLI_OPT_NUMERIC_AND_DATETIME_AS_UNICODE, 1)))
		printf("[017] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, 'SELECT * FROM test'))
		printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!is_unicode($row['col_time']) || '' == $row['col_time'])
		printf("[020] Expecting unicode/any, got %s/%s\n", gettype($row['col_time']), $row['col_time']);

	mysqli_free_result($res);

	if (!$stmt = mysqli_stmt_init($link))
		printf("[021] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$col_date = $col_time = $col_datetime = $col_timestamp = null;
	if (!mysqli_stmt_prepare($stmt, 'SELECT col_date, col_time, col_datetime, col_timestamp FROM test') ||
		!mysqli_stmt_execute($stmt) ||
		!mysqli_stmt_bind_result($stmt, $col_date, $col_time, $col_datetime, $col_timestamp) ||
		!mysqli_stmt_fetch($stmt))
		printf("[022] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_unicode($col_date) || '' == $col_date)
		printf("[023] Expecting unicode/any, got %s/%s\n", gettype($col_date), $col_date);

	if (!is_unicode($col_time) || '' == $col_time)
		printf("[024] Expecting unicode/any, got %s/%s\n", gettype($col_time), $col_time);

	if (!is_unicode($col_datetime) || '' == $col_datetime)
		printf("[025] Expecting unicode/any, got %s/%s\n", gettype($col_datetime), $col_datetime);

	if (!is_unicode($col_timestamp) || '' == $col_timestamp)
		printf("[026] Expecting unicode/any, got %s/%s\n", gettype($col_timestamp), $col_timestamp);

	mysqli_stmt_close($stmt);

	if (true !== ($tmp = mysqli_set_opt($link, MYSQLI_OPT_NUMERIC_AND_DATETIME_AS_UNICODE, 0)))
		printf("[027] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!$stmt = mysqli_stmt_init($link))
		printf("[028] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$col_date = $col_time = $col_datetime = $col_timestamp = null;
	if (!mysqli_stmt_prepare($stmt, 'SELECT col_date, col_time, col_datetime, col_timestamp FROM test') ||
		!mysqli_stmt_execute($stmt) ||
		!mysqli_stmt_bind_result($stmt, $col_date, $col_time, $col_datetime, $col_timestamp) ||
		!mysqli_stmt_fetch($stmt))
		printf("[029] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (is_unicode($col_date) || '' == $col_date)
		printf("[030] Expecting (binary) string/any, got %s/%s\n", gettype($col_date), $col_date);

	if (is_unicode($col_time) || '' == $col_time)
		printf("[031] Expecting (binary) string/any, got %s/%s\n", gettype($col_time), $col_time);

	if (is_unicode($col_datetime) || '' == $col_datetime)
		printf("[032] Expecting (binary) string/any, got %s/%s\n", gettype($col_datetime), $col_datetime);

	if (is_unicode($col_timestamp) || '' == $col_timestamp)
		printf("[033] Expecting (binary) string/any, got %s/%s\n", gettype($col_timestamp), $col_timestamp);

	mysqli_stmt_close($stmt);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
done!