--TEST--
Notice triggered by invalid configuration options
--CREDITS--
Christian Wenz <wenz@php.net>
--EXTENSIONS--
tidy
--FILE--
<?php
$buffer = '<html></html>';
$config = ['bogus' => 'willnotwork'];

$tidy = new tidy();

try {
	$tidy->parseString($buffer, $config);
} catch (\Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

$config = ['neither'];
try {
	$tidy->parseString($buffer, $config);
} catch (\Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
tidy::parseString(): Argument #2 ($config) Unknown Tidy configuration option "bogus"
tidy::parseString(): Argument #2 ($config) must be of type array with keys as string
