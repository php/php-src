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
} catch (\ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage(), PHP_EOL;
}

$config = ['neither'];
try {
	$tidy->parseString($buffer, $config);
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}

$config = ['doctype-mode' => 'customtag'];

try {
	var_dump($tidy->parseString($buffer, $config));
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

$config = ['doctype' => 'php', 0 => 'value2'];

try {
	var_dump($tidy->parseString($buffer, $config));
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: tidy::parseString(): Argument #2 ($config) Unknown Tidy configuration option "bogus"
tidy::parseString(): Argument #2 ($config) must be of type array with keys as string
tidy::parseString(): Argument #2 ($config) Attempting to set read-only option "doctype-mode"
tidy::parseString(): Argument #2 ($config) must be of type array with keys as string
