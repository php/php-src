--TEST--
mysqli_query() - unicode (cyrillic)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
require_once('table.inc');
if (!$res = mysqli_query($link, "SHOW CHARACTER SET LIKE 'utf8'"))
	die("skip UTF8 chatset seems not available");
mysqli_free_result($res);
mysqli_close($link);
?>
--FILE--
<?php
	include_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_query()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_query($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require_once('table.inc');

	if (TRUE !== ($tmp = @mysqli_query($link, "set names utf8")))
		printf("[002.5] Expecting TRUE, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_query($link, "SELECT 1 AS колона", MYSQLI_USE_RESULT, "foo")))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (false !== ($tmp = mysqli_query($link, 'това не е ескюел')))
		printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (false !== ($tmp = mysqli_query($link, "SELECT 'това е ескюел, но със обратна наклонена и g'\g")))
		printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if ((0 === mysqli_errno($link)) || ('' == mysqli_error($link)))
		printf("[006] mysqli_errno()/mysqli_error should return some error\n");

	if (!$res = mysqli_query($link, "SELECT 'това ескюел, но с точка и запетая' AS правилен ; "))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	var_dump(mysqli_fetch_assoc($res));
	mysqli_free_result($res);

	if (false !== ($res = mysqli_query($link, "SELECT 'това ескюел, но с точка и запетая' AS правилен ; SHOW VARIABLES")))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (mysqli_get_server_version($link) > 50000) {
		// let's try to play with stored procedures
		mysqli_query($link, 'DROP PROCEDURE IF EXISTS процедурка');
		if (mysqli_query($link, 'CREATE PROCEDURE процедурка(OUT версия VARCHAR(25)) BEGIN SELECT VERSION() INTO версия; END;')) {
			$res = mysqli_query($link, 'CALL процедурка(@version)');
			$res = mysqli_query($link, 'SELECT @version AS п_версия');

			$tmp = mysqli_fetch_assoc($res);
			if (!is_array($tmp) || empty($tmp) || !isset($tmp['п_версия']) || ('' == $tmp['п_версия'])) {
				printf("[008a] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				var_dump($tmp);
			}

			mysqli_free_result($res);
		} else {
			printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		}

		mysqli_query($link, 'DROP FUNCTION IF EXISTS функцийка');
		if (mysqli_query($link, 'CREATE FUNCTION функцийка( параметър_версия VARCHAR(25)) RETURNS VARCHAR(25) DETERMINISTIC RETURN параметър_версия;')) {
			$res = mysqli_query($link, 'SELECT функцийка(VERSION()) AS ф_версия');

			$tmp = mysqli_fetch_assoc($res);
			if (!is_array($tmp) || empty($tmp) || !isset($tmp['ф_версия']) || ('' == $tmp['ф_версия'])) {
				printf("[009a] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				var_dump($tmp);
			}

			mysqli_free_result($res);
		} else {
			printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		}
	}

	/*
	Trying to test what Ramil suggests in http://bugs.mysql.com/bug.php?id=29576
	However, this won't work, because we're lacking MYSQLI_SET_CHARSET_NAME.
	if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1)) {
		if (mysqli_get_server_version() > 50002) {
			@mysqli_query($link, "DROP USER IF EXISTS 'тест'@'%'");
			if (TRUE !== mysqli_query($link, "CREATE USER 'тест'@'%'")) {
				var_dump(mysqli_error($link);
			}
		}
		if (TRUE !== mysqli_query($link, "GRANT SELECT ON $db.* TO 'тест'@'%' IDENTIFIED BY 'парола'")) {
			var_dump(mysqli_error($link);
		} else {
			$link2 = mysqli_init();
			if (!(mysqli_real_connect($link2, $host, "тест", 'парола', $db, $port, $socket))) {
				printf("[011] Cannot connect to the server using host=%s, user=%s, passwd=парола, dbname=%s, port=%s, socket=%s, [%d] %s\n",
					$host, "тест", $db, $port, $socket,
					mysqli_connect_errno(), mysqli_connect_error());
			}
			mysqli_close($link2);
			if (mysqli_get_server_version() > 50002) {
				if (!mysqli_query($link, "DROP USER 'тест'@'%'"))
					printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				} else {
					printf("[015] Cannot create user or grant privileges, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				}
			}
		}
	}
	*/

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_query($link, "SELECT id FROM test")))
		printf("[014] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
array(1) {
  [%u|b%"правилен"]=>
  %unicode|string%(%d) "това ескюел, но с точка и запетая"
}

Warning: mysqli_query(): Couldn't fetch mysqli in %s on line %d
done!