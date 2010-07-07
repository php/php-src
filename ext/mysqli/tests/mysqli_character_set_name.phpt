--TEST--
mysqli_chararcter_set_name(), mysql_client_encoding() [alias]
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	/* NOTE: http://bugs.mysql.com/bug.php?id=7923 makes this test fail very likely on all 4.1.x - 5.0.x! */
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_character_set_name()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_character_set_name($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_character_set_name($link, $link, $link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[005] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!$res = mysqli_query($link, 'SELECT version() AS server_version'))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	$version = explode('.', $tmp['server_version']);
	if (empty($version))
		printf("[006] Cannot determine server version, need MySQL Server 4.1+ for the test!\n");

	if ($version[0] <= 4 && $version[1] < 1)
		printf("[007] Need MySQL Server 4.1+ for the test!\n");

	if (!$res = mysqli_query($link, 'SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
			printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if (!$tmp['charset'])
		printf("[009] Cannot determine current character set and collation\n");

	$charset = mysqli_character_set_name($link);
	if ($tmp['charset'] !== $charset) {
		if ($tmp['collation'] === $charset) {
			printf("[010] Could be known server bug http://bugs.mysql.com/bug.php?id=7923, collation %s instead of character set returned, expected string/%s, got %s/%s\n",
			$tmp['collation'], $tmp['charset'], gettype($charset), $charset);
		} else {
			printf("[011] Expecting character set %s/%s, got %s/%s\n", gettype($tmp['charset']), $tmp['charset'], gettype($charset), $charset);
		}
	}

	$charset2 = mysqli_character_set_name($link);
	if ($charset2 !== $charset) {
		printf("[012] Alias mysqli_character_set_name returned %s/%s, expected  %s/%s\n", gettype($charset2), $charset2, gettype($charset), $charset);
	}

	mysqli_close($link);

	if (NULL !== ($tmp = @mysqli_character_set_name($link)))
		printf("[013] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	/* Make sure that the function alias exists */
	if (!is_null($tmp = @mysqli_character_set_name()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
done!