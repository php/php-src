--TEST--
mysqli_set_local_infile_handler() - do not use the file pointer
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
?>
--FILE--
<?php
	require_once('connect.inc');
	require_once('local_infile_tools.inc');
	require_once('table.inc');

	function callback_unregister($fp, &$buffer, $buflen, &$error) {
		global $link;
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);
		flush();
		if (is_resource($fp))
				fclose($fp);
		$buffer = "1;'a';\n";
		if ($invocation > 10)
			return 0;

		mysqli_set_local_infile_default($link);
		return strlen($buffer);
	}

	$file = create_standard_csv(1);
	$expected = array(array('id' => 1, 'label' => 'a'));
	try_handler(20, $link, $file, 'callback_unregister', $expected);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
Callback set to 'callback_unregister'
Callback: 0
[022] LOAD DATA failed, [2000] Can't execute load data local init callback function
done!