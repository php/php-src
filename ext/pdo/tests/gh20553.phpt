--TEST--
GH-20553: PHP 8.5.0 regression: PDO::FETCH_CLASSTYPE ignores $constructorArgs
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (str_starts_with(getenv('PDOTEST_DSN'), "firebird")) {
	die("skip firebird doesn't support non-standard SQL without FROM syntax");
}
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

class dumpy {
	function __construct() {
		echo 'constructor called,' . PHP_EOL . '  my class is ' .
			var_export(get_class($this), true) . PHP_EOL .
			'  input parameters: ' .
			var_export(func_get_args(), true) . PHP_EOL;
	}
	function __set($name, $value) {
		echo var_export($name, true) . ' = ' . var_export($value, true) .
			PHP_EOL;
	}
}
class dummy extends dumpy {
}

$sql = "SELECT 'dummy' pdo_fetch_class_type_class, 'bar' foo, 'dfg' abc";

$fetchModes = [
	'PDO::FETCH_CLASS'
		=> PDO::FETCH_CLASS,
	'PDO::FETCH_CLASS | PDO::FETCH_PROPS_LATE'
		=> PDO::FETCH_CLASS | PDO::FETCH_PROPS_LATE,
	'PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE'
		=> PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE,
	'PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE | PDO::FETCH_PROPS_LATE'
		=> PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE | PDO::FETCH_PROPS_LATE,
];

foreach ($fetchModes as $combinedModes => $fetchMode) {
	echo '## ' . $combinedModes . PHP_EOL;
	$db->query($sql)->fetchAll(
		$fetchMode,
		'dumpy',
		['constructor argument #1']
	);
	echo PHP_EOL;
}
?>
--EXPECT--
## PDO::FETCH_CLASS
'pdo_fetch_class_type_class' = 'dummy'
'foo' = 'bar'
'abc' = 'dfg'
constructor called,
  my class is 'dumpy'
  input parameters: array (
  0 => 'constructor argument #1',
)

## PDO::FETCH_CLASS | PDO::FETCH_PROPS_LATE
constructor called,
  my class is 'dumpy'
  input parameters: array (
  0 => 'constructor argument #1',
)
'pdo_fetch_class_type_class' = 'dummy'
'foo' = 'bar'
'abc' = 'dfg'

## PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE
'foo' = 'bar'
'abc' = 'dfg'
constructor called,
  my class is 'dummy'
  input parameters: array (
  0 => 'constructor argument #1',
)

## PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE | PDO::FETCH_PROPS_LATE
constructor called,
  my class is 'dummy'
  input parameters: array (
  0 => 'constructor argument #1',
)
'foo' = 'bar'
'abc' = 'dfg'
