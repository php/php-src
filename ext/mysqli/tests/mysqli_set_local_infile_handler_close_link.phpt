--TEST--
mysqli_set_local_infile_handler() - close database link
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
	require("table.inc");
	require_once('local_infile_tools.inc');

	function callback_close_link($fp, &$buffer, $buflen, &$error) {
		global $link;
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);
		flush();
		if (is_object($link))
			mysqli_close($link);

		$buffer = "1;'a';\n";
		if ($invocation > 10)
			return 0;

		return strlen($buffer);
	}

	$file = create_standard_csv(1);
	$expected = array(array('id' => 1, 'label' => 'a'));
	try_handler(20, $link, $file, 'callback_close_link', $expected);
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Callback set to 'callback_close_link'
Callback: 0
done!