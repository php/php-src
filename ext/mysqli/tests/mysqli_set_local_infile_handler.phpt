--TEST--
mysqli_set_local_infile_handler()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_set_local_infile_handler'))
	die("skip - function not available.");

require_once('connect.inc');
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

	function callback_simple($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation);

		$invocation++;
		if (!is_resource($fp))
			printf("[012] First argument passed to callback is not a resource but %s/%s\n",
				$fp, gettype($fp));

		if (!$buffer = fread($fp, $buflen)) {
			if ($invocation == 1) {
				printf("[013] Cannot read from stream\n");
					$error = 'Cannot read from stream';
			} else {
				return strlen($buffer);
			}
		}

		$lines = explode("\n", $buffer);
		if (count($lines) != 4 && strlen($buffer) > 0) {
			printf("[014] Test is too simple to handle a buffer of size %d that cannot hold all lines\n", $buflen);
			$error = 'Parser too simple';
		}

		$buffer = '';
		foreach ($lines as $k => $line) {
			if ('' === trim($line))
				continue;

			$columns = explode(';', $line);
			if (empty($columns)) {
				printf("[015] Cannot parse columns\n");
				$error = 'Cannot parse columns';
			}

			// increase id column value
			$columns[0] += 1;
			$buffer .= implode(';', $columns);
			$buffer .= "\n";
		}

		return strlen($buffer);
	}

	function callback_fclose($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);

		fclose($fp);
		return strlen($buffer);
	}

	function callback_closefile($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);
		flush();
		if (is_resource($fp))
			fclose($fp);
		$buffer = "1;'a';\n";
		if ($invocation > 10)
			return 0;

		return strlen($buffer);
	}

	function callback_invalid_args($fp, &$buffer, $buflen) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);
		$buffer = fread($fp, $buflen);

		return strlen($buffer);
	}

	function callback_error($fp, &$buffer, $buflen, &$error) {
		static $invocation = 0;

		printf("Callback: %d\n", $invocation++);
		$buffer = fread($fp, $buflen);
		$error = 'How to access this error?';

		return -1;
	}

	if (!is_null($tmp = @mysqli_set_local_infile_handler()))
		printf("[001] Expecting NULL/NULL got %s/%s\n", $tmp, gettype($tmp));

	$handle = null;
	if (!is_null($tmp = @mysqli_set_local_infile_handler($handle)))
		printf("[002] Expecting NULL/NULL got %s/%s\n", $tmp, gettype($tmp));

	$handle = @new mysqli();
	if (!is_null($tmp = @mysqli_set_local_infile_handler($handle, 'callback_simple')))
		printf("[003] Expecting NULL/NULL got %s/%s\n", $tmp, gettype($tmp));

	if (false !== ($tmp = @mysqli_set_local_infile_handler($link, 'unknown')))
		printf("[004] Expecting false/boolean got %s/%s\n", $tmp, gettype($tmp));

	$file = create_standard_csv(5);

	$expected = array(
		array('id' => 98,   'label' => 'x'),
		array('id' => 99,   'label' => 'y'),
		array('id' => 100,  'label' => 'z'),
	);
	try_handler(10, $link, $file, 'callback_simple', $expected);

	$expected = array();
	try_handler(20, $link, $file, 'callback_fclose', $expected);

	// FIXME - TODO - KLUDGE -
  // IMHO this is wrong. ext/mysqli should bail as the function signature
  // is not complete. That's a BC break, OK, but it makes perfectly sense.
	$expected = array();
	try_handler(30, $link, $file, 'callback_invalid_args', $expected);

	$expected = array();
	try_handler(40, $link, $file, 'callback_error', $expected);


	mysqli_close($link);

	if (!is_null($tmp = @mysqli_set_local_infile_handler($link, 'callback_simple')))
		printf("[300] Expecting NULL/NULL got %s/%s\n", $tmp, gettype($tmp));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Callback set to 'callback_simple'
Callback: 0
Callback: 1
Callback set to 'callback_fclose'
Callback: 0
[022] LOAD DATA failed, [2000] File handle close%s
Callback set to 'callback_invalid_args'
Callback: 0
Callback: 1
[037] More results than expected!
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(2) "97"
  [%u|b%"label"]=>
  %unicode|string%(1) "x"
}
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(2) "98"
  [%u|b%"label"]=>
  %unicode|string%(1) "y"
}
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(2) "99"
  [%u|b%"label"]=>
  %unicode|string%(1) "z"
}
Callback set to 'callback_error'
Callback: 0
[042] LOAD DATA failed, [2000] How to access this error?
done!