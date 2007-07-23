--TEST--
mysqli_set_charset()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php
require_once('skipifemb.inc');
if (!function_exists('mysqli_set_charset'))
 	die("skip Function not available");
if (ini_get("unicode.semantics"))
	die("skip: mysqli_set_charset() is disabled in unicode");
?>
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_set_charset()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_set_charset($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_set_charset($link, $link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$res = mysqli_query($link, 'SELECT version() AS server_version'))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	$version = explode('.', $tmp['server_version']);
	if (empty($version))
		printf("[005] Cannot determine server version, need MySQL Server 4.1+ for the test!\n");

	if ($version[0] <= 4 && $version[1] < 1)
		printf("[006] Need MySQL Server 4.1+ for the test!\n");

	if (!$res = mysqli_query($link, 'SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if (!$character_set_connection = $tmp['charset'])
		printf("[008] Cannot determine current character set and collation\n");

	$new_charset = ('latin1' == $character_set_connection) ? 'latin2' : 'latin1';
	if (!$res = mysqli_query($link, sprintf('SHOW CHARACTER SET LIKE "%s"', $new_charset), MYSQLI_STORE_RESULT))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (mysqli_num_rows($res) == 0)
		printf("[010] Test will fail, because alternative test character set '%s' seems not supported\n", $new_charset);

	if (false !== ($ret = mysqli_set_charset($link, "this is not a valid character set")))
		printf("[011] Expecting boolean/false because of invalid character set, got %s/%s\n", gettype($ret), $ret);

	mysqli_close($link);
	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[012] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (true !== ($ret = mysqli_set_charset($link, $new_charset)))
		printf("[013] Expecting boolean/true, got %s/%s\n", gettype($ret), $ret);

	if (!$res = mysqli_query($link, 'SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
		printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if ($new_charset !== $tmp['charset'])
		printf("[015] Character set not changed? Expecting %s, got %s\n", $new_charset, $tmp['charset']);

	if (!$res = mysqli_query($link, "SHOW CHARACTER SET"))
		printf("[016] Cannot get list of character sets\n");

	while ($tmp = mysqli_fetch_assoc($res)) {
		if ('ucs2' == $tmp['Charset'])
			continue;

		/* Uncomment to see where it hangs - var_dump($tmp); flush(); */
		if (!@mysqli_set_charset($link, $tmp['Charset'])) {
			printf("[017] Cannot set character set to '%s', [%d] %s\n", $tmp['Charset'],
				mysqli_errno($link), mysqli_error($link));
			continue;
		}

		/* Uncomment to see where it hangs - var_dump($tmp); flush(); */
		if (!mysqli_query($link, sprintf("SET NAMES %s", mysqli_real_escape_string($link, $tmp['Charset']))))
			printf("[018] Cannot run SET NAMES %s, [%d] %s\n", $tmp['Charset'], mysqli_errno($link), mysqli_error($link));
	}
	mysqli_free_result($res);

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_set_charset($link, $new_charset)))
		printf("[016] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
Warning: mysqli_set_charset(): Couldn't fetch mysqli in %s on line %d
done!