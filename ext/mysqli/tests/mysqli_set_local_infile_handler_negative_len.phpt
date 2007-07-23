--TEST--
mysqli_set_local_infile_handler() - negative return value/buflen to indicate an error
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');

if (!function_exists('mysqli_set_local_infile_handler'))
	die("skip - function not available.");

require_once('connect.inc');
if (!$TEST_EXPERIMENTAL)
	die("skip - experimental (= unsupported) feature");
?>
--FILE--
<?php
	require_once('connect.inc');
	require_once('local_infile_tools.inc');
	require_once('table.inc');

	function callback_negative_len($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation);
		$buffer = fread($fp, $buflen);

		$error = "negative length means error";
		return -1;
	}

	$file = create_standard_csv(1);
	$expected = array();
	try_handler(20, $link, $file, 'callback_negative_len', $expected);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
Callback set to 'callback_negative_len'
Callback: 0

Warning: mysqli_query(): negative length means error in %s on line %d
[022] LOAD DATA failed, [2000] negative length means error
done!