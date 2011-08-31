--TEST--
mysqli_set_local_infile_handler() - buffer overflow
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_set_local_infile_handler'))
	die("skip - function not available.");

require_once('connect.inc');
if (!$TEST_EXPERIMENTAL)
	die("skip - experimental (= unsupported) feature");

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die("skip Cannot connect to MySQL");

include_once("local_infile_tools.inc");
if ($msg = check_local_infile_support($link, $engine))
	die(sprintf("skip %s, [%d] %s", $msg, $link->errno, $link->error));

mysqli_close($link);
?>
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php
	require_once('connect.inc');
	require_once('local_infile_tools.inc');
	require_once('table.inc');

	function callback_buffer_overflow($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation);
		$buffer = fread($fp, $buflen);

		$buffer = str_repeat(';', $buflen * 2);
		return strlen($buffer);
	}

	$file = create_standard_csv(5);
	$expected = array();
	try_handler(20, $link, $file, 'callback_buffer_overflow', $expected);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Callback set to 'callback_buffer_overflow'
Callback: 0

Warning: mysqli_query(): Too much data returned in %s on line %d
[022] LOAD DATA failed, [%d] Too much data returned
done!