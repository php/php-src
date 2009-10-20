--TEST--
mysqli_set_local_infile_handler() - random ASCII character including \0
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!function_exists('mysqli_set_local_infile_handler'))
    die("skip - function not available.");

if (!$TEST_EXPERIMENTAL)
    die("skip - experimental (= unsupported) feature");

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die("skip Cannot connect to MySQL");

if (!$res = mysqli_query($link, 'SHOW VARIABLES LIKE "local_infile"')) {
	mysqli_close($link);
	die("skip Cannot check if Server variable 'local_infile' is set to 'ON'");
}

$row = mysqli_fetch_assoc($res);
mysqli_free_result($res);
mysqli_close($link);

if ('ON' != $row['Value'])
	die(sprintf("skip Server variable 'local_infile' seems not set to 'ON', found '%s'",
		$row['Value']));
?>
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php
	require_once('connect.inc');
	require_once('local_infile_tools.inc');
	require_once('table.inc');

	function callback_bad_character($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);

		$num_chars = (version_compare(PHP_VERSION, '5.9.9', '>') == 1) ? (floor($buflen / 2) - 10) : ($buflen - 5);
		$part1 = floor($num_chars / 2);
		$part2 = $num_chars - $part1;

		$buffer = '';
		for ($i = 0; $i < $part1; $i++)
			$buffer .= chr(mt_rand(0, 255));

		$buffer .= ';"';

		for ($i = 0; $i < $part2; $i++)
			$buffer .= chr(mt_rand(0, 255));

		$buffer .= '";';
		if ($invocation > 10)
			return 0;

		return strlen($buffer);
	}

	$file = create_standard_csv(5);
	/* we feed the handler with random data, therefore we cannot specify and expected rows */
	try_handler(20, $link, $file, 'callback_bad_character');

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Callback set to 'callback_bad_character'
Callback: 0
Callback: 1
Callback: 2
Callback: 3
Callback: 4
Callback: 5
Callback: 6
Callback: 7
Callback: 8
Callback: 9
Callback: 10
done!