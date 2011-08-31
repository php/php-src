--TEST--
mysqli_set_local_infile_default()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_set_local_infile_handler'))
	die("skip - function not available.");

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (!$link)
	die(sprintf("skip Can't connect [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

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

	$link = $tmp = null;
	if (!is_null($tmp = @mysqli_set_local_infile_default()))
		printf("[001] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_set_local_infile_default($link)))
		printf("[002] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	$link = new mysqli();
	if (!is_null($tmp = @mysqli_set_local_infile_default($link)))
		printf("[002a] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	include("table.inc");

	if (!is_null($tmp = @mysqli_set_local_infile_default($link, 'foo')))
		printf("[003] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);


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

	$file = create_standard_csv(4);
	$expected = array(
		array('id' => 98,   'label' => 'x'),
		array('id' => 99,   'label' => 'y'),
		array('id' => 100,  'label' => 'z'),
	);
	try_handler(10, $link, $file, 'callback_simple', $expected);

	$expected = array(
		array('id' => 97,   'label' => 'x'),
		array('id' => 98,   'label' => 'y'),
		array('id' => 99,   'label' => 'z'),
	);
	try_handler(20, $link, $file, 'default', $expected);

	$expected = array(
		array('id' => 98,   'label' => 'x'),
		array('id' => 99,   'label' => 'y'),
		array('id' => 100,  'label' => 'z'),
	);
	try_handler(30, $link, $file, 'callback_simple', $expected);

	mysqli_close($link);

	if (!is_null($tmp = @mysqli_set_local_infile_default($link)))
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
Callback set to 'default'
Callback set to 'callback_simple'
Callback: 2
Callback: 3
done!