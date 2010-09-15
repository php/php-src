--TEST--
magic_quotes_runtime (DEPRECATED)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (version_compare(PHP_VERSION, '5.3.98') >= 0) {
	die("skip: PHP 5.3 test");
}
?>
--INI--
magic_quotes_runtime=1
--FILE--
<?php
	require('connect.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	$query = sprintf("SELECT '%s', '%s', '%s', '%s' FROM DUAL",
				mysqli_real_escape_string($link, "'"),
				mysqli_real_escape_string($link, '"'),
				mysqli_real_escape_string($link, '\0'),
				mysqli_real_escape_string($link, '\\'));

	if (!$res = mysqli_query($link, $query)) {
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	while ($row = $res->fetch_assoc()) {
		var_dump($row);
	}

	$res->free();

	$expected = array(
		"'" 	=> "\\'",
		'"' 	=> "\\\"",
		"\\0" 	=> "\\\\0",
		"\\"	=> "\\\\",
	);
	$expectedBoth = array(
		0		=> "\\'",
		"'" 	=> "\\'",
		1	 	=> "\\\"",
		'"' 	=> "\\\"",
		2	 	=> "\\\\0",
		"\\0" 	=> "\\\\0",
		3		=> "\\\\",
		"\\"	=> "\\\\",
	);

	if (!$res = mysqli_query($link, $query)) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	$row = mysqli_fetch_row($res);
	echo "Equal:";var_dump($row === array_values($expected));
	if ($row !== array_values($expected)) {
		var_dump($row, array_values($expected));
	}
	$res->free();

	if (!$res = mysqli_query($link, $query)) {
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	$row = mysqli_fetch_array($res, MYSQLI_BOTH);
	echo "Equal:";var_dump($row === $expectedBoth);
	if ($row !== $expectedBoth) {
		var_dump($row, $expectedBoth);
	}
	$res->free();

	class fetch_object {
		public function __set($key, $value) {
			printf(">%s< => >%s<\n", $key, $value);
		}
	}

	if (!$res = mysqli_query($link, $query)) {
		printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	$obj = mysqli_fetch_object($res, "fetch_object");
	$res->free();

	if (false && $IS_MYSQLND) {
			if (!$res = mysqli_query($link, $query)) {
				printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
			}
			$row = @mysqli_fetch_all($res, MYSQLI_ASSOC);
			if ($row[0] !== $expected) {
				printf("[015] Wrong data, dumping\n");
				var_dump($row);
			}
	}

	$link->close();

	print "done!";
?>
--EXPECTF--
Deprecated: Directive 'magic_quotes_runtime' is deprecated in PHP 5.3 and greater in Unknown on line %d

Warning: mysqli_result::fetch_assoc(): magic_quotes_runtime are deprecated since PHP 5.3 in %s on line %d
array(4) {
  ["'"]=>
  string(2) "\'"
  ["""]=>
  string(2) "\""
  ["\0"]=>
  string(3) "\\0"
  ["\"]=>
  string(2) "\\"
}

Warning: mysqli_fetch_row(): magic_quotes_runtime are deprecated since PHP 5.3 in %s on line %d
Equal:bool(true)

Warning: mysqli_fetch_array(): magic_quotes_runtime are deprecated since PHP 5.3 in %s on line %d
Equal:bool(true)

Warning: mysqli_fetch_object(): magic_quotes_runtime are deprecated since PHP 5.3 in %s on line %d
>'< => >\'<
>"< => >\"<
>\0< => >\\0<
>\< => >\\<
done!
