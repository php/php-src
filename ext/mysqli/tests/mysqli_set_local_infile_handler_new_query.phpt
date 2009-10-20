--TEST--
mysqli_set_local_infile_handler() - run new query on db link
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
	require_once('connect.inc');
	require_once('local_infile_tools.inc');
	require_once('table.inc');

	function callback_new_query($fp, &$buffer, $buflen, &$error) {
		global $link;
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);
		flush();
		if (is_object($link)) {
			if (!$res = mysqli_query($link, "SELECT id, label FROM test")) {
				printf("[Callback 001 - %03d] Cannot run query, [%d] %s\n",
					$invocation, mysqli_errno($link), mysqli_error($link));
			}
			if ($res)
				mysqli_free_result($res);
			}
			$buffer = "1;'a';\n";
			if ($invocation > 10)
				return 0;

			mysqli_set_local_infile_default($link);
			return strlen($buffer);
	}

	$file = create_standard_csv(1);
	$expected = array(array('id' => 1, 'label' => 'a'));
	try_handler(20, $link, $file, 'callback_new_query', $expected);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Callback set to 'callback_new_query'
Callback: 0
[Callback 001 - 001] Cannot run query, [2014] Commands out of sync; you can't run this command now
[022] LOAD DATA failed, [2000] Can't execute load data local init callback function
done!