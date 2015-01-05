--TEST--
mysqli_get_charset()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_get_charset'))
	die("skip: function not available");
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_get_charset()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_get_charset($link)))
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
	if (!($character_set_connection = $tmp['charset']) || !($collation_connection = $tmp['collation']))
		printf("[008] Cannot determine current character set and collation\n");

	if (!$res = mysqli_query($link, $sql = sprintf("SHOW CHARACTER SET LIKE '%s'", $character_set_connection)))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	if (empty($tmp))
		printf("[010] Cannot fetch Maxlen and/or Comment, test will fail: $sql\n");

	$maxlen = (isset($tmp['Maxlen'])) ? $tmp['Maxlen'] : '';
	$comment = (isset($tmp['Description'])) ? $tmp['Description'] : '';

	if (!$res = mysqli_query($link, sprintf("SHOW COLLATION LIKE '%s'", $collation_connection)))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if (!($id = $tmp['Id']))
		printf("[012] Cannot fetch Id/Number, test will fail\n");

	if (!$res = mysqli_query($link, sprintf("SHOW VARIABLES LIKE 'character_sets_dir'")))
		printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if (!($character_sets_dir = $tmp['Value']))
		printf("[014] Cannot fetch character_sets_dir, test will fail\n");

	if (!is_object($charset = mysqli_get_charset($link)))
		printf("[015] Expecting object/std_class, got %s/%s\n", gettype($charset), $charset);

	if (!isset($charset->charset) ||
		!in_array(gettype($charset->charset), array("string", "unicode")) ||
		($character_set_connection !== $charset->charset))
		printf("[016] Expecting string/%s, got %s/%s\n", $character_set_connection, gettype($charset->charset), $charset->charset);
	if (!isset($charset->collation) ||
		!in_array(gettype($charset->collation), array("string", "unicode")) ||
		($collation_connection !== $charset->collation))
		printf("[017] Expecting string/%s, got %s/%s\n", $collation_connection, gettype($charset->collation), $charset->collation);

	if (!isset($charset->dir) ||
		!is_string($charset->dir))
		printf("[019] Expecting string - ideally %s*, got %s/%s\n", $character_sets_dir, gettype($charset->dir), $charset->dir);

	if (!isset($charset->min_length) ||
		!(is_int($charset->min_length)) ||
		($charset->min_length < 0) ||
		($charset->min_length > $charset->max_length))
		printf("[020] Expecting int between 0 ... %d, got %s/%s\n", $charset->max_length,
			gettype($charset->min_length), $charset->min_length);

	if (!isset($charset->number) ||
		!is_int($charset->number) ||
		($charset->number !== (int)$id))
		printf("[021] Expecting int/%d, got %s/%s\n", $id, gettype($charset->number), $charset->number);

	if (!isset($charset->state) ||
		!is_int($charset->state))
		printf("[022] Expecting int/any, got %s/%s\n", gettype($charset->state), $charset->state);

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_get_charset($link)))
		printf("[023] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_get_charset(): Couldn't fetch mysqli in %s on line %d
done!
