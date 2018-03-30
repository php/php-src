--TEST--
mysqli_change_user() - SET NAMES
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die(sprintf("skip [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!$res = mysqli_query($link, 'SELECT version() AS server_version'))
	die(sprintf("skip [%d] %s\n", mysqli_errno($link), mysqli_error($link)));

$tmp = mysqli_fetch_assoc($res);
mysqli_free_result($res);
$version = explode('.', $tmp['server_version']);
if (empty($version))
	die(sprintf("skip Cannot determine server version, we need MySQL Server 4.1+ for the test!"));

if ($version[0] <= 4 && $version[1] < 1)
	die(sprintf("skip We need MySQL Server 4.1+ for the test!"));
?>
--FILE--
<?php
	require_once('connect.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (!$res = mysqli_query($link, "SHOW CHARACTER SET LIKE 'latin%'"))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$charsets = array();
	while ($row = mysqli_fetch_assoc($res))
		$charsets[$row['Charset']] = $row['Default collation'];

	mysqli_free_result($res);
	if (!mysqli_query($link, 'SET NAMES DEFAULT'))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT
		@@character_set_client AS charset_client,
		@@character_set_connection AS charset_connection,
		@@character_set_results AS charset_results,
		@@collation_connection AS collation_connection,
		@@collation_database AS collation_database,
		@@collation_server AS collation_server'))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$defaults = mysqli_fetch_assoc($res))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_free_result($res);

	$not_changed = $defaults;
	foreach ($charsets as $charset => $collation) {

		if (isset($not_changed['charset_client']) &&
				$charset != $not_changed['charset_client'] &&
				mysqli_query($link, sprintf("SET @@character_set_client = '%s'", $charset)))
			unset($not_changed['charset_client']);

		if (isset($not_changed['charset_connection']) &&
				$charset != $not_changed['charset_connection'] &&
				mysqli_query($link, sprintf("SET @@character_connection = '%s'", $charset)))
			unset($not_changed['charset_connection']);

		if (isset($not_changed['charset_results']) &&
				$charset != $not_changed['charset_results'] &&
				mysqli_query($link, sprintf("SET @@character_set_results = '%s'", $charset)))
			unset($not_changed['charset_results']);

		if (isset($not_changed['collation_connection']) &&
				$collation != $not_changed['collation_connection'] &&
				mysqli_query($link, sprintf("SET @@collation_connection = '%s'", $collation)))
			unset($not_changed['collation_connection']);

		if (isset($not_changed['collation_database']) &&
				$collation != $not_changed['collation_database'] &&
				mysqli_query($link, sprintf("SET @@collation_database = '%s'", $collation)))
			unset($not_changed['collation_database']);

		if (isset($not_changed['collation_server']) &&
				$collation != $not_changed['collation_server'] &&
				mysqli_query($link, sprintf("SET @@collation_server = '%s'", $collation)))
			unset($not_changed['collation_server']);

		if (empty($not_changed))
			break;
	}

	if (!$res = mysqli_query($link, 'SELECT
		@@character_set_client AS charset_client,
		@@character_set_connection AS charset_connection,
		@@character_set_results AS charset_results,
		@@collation_connection AS collation_connection,
		@@collation_database AS collation_database,
		@@collation_server AS collation_server'))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$modified = mysqli_fetch_assoc($res))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_free_result($res);

	if ($modified == $defaults)
		printf("[008] Not all settings have been changed\n");

	// LAST_INSERT_ID should be reset
	mysqli_change_user($link, $user, $passwd, $db);

	if (!$res = mysqli_query($link, 'SELECT
		@@character_set_client AS charset_client,
		@@character_set_connection AS charset_connection,
		@@character_set_results AS charset_results,
		@@collation_connection AS collation_connection,
		@@collation_database AS collation_database,
		@@collation_server AS collation_server'))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$new = mysqli_fetch_assoc($res))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_free_result($res);

	if ($new == $modified) {
		printf("[011] Charsets/collations have not been reset.\n");
		printf("Got:\n");
		var_dump($new);
		printf("Expected:\n");
		var_dump($defaults);
	}

	if ($new != $defaults) {
		printf("[012] Charsets/collations have not been reset to their defaults.\n");
		printf("Got:\n");
		var_dump($new);
		printf("Expected:\n");
		var_dump($defaults);
	}

	if (!is_object($charset = mysqli_get_charset($link)))
		printf("[013] Expecting object/std_class, got %s/%s\n", gettype($charset), $charset);

	if ($charset->charset != $defaults['charset_connection'])
		printf("[014] Expecting connection charset to be %s got %s\n",
			$defaults['charset_connection'],
			$charset->charset);

	if ($charset->collation != $defaults['collation_connection'])
		printf("[015] Expecting collation to be %s got %s\n",
			$defaults['collation_connection'],
			$charset->collation);

	mysqli_close($link);
	print "done!";
?>
--EXPECT--
done!
