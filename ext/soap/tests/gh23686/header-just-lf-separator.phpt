--TEST--
GH-23686: Confirm that headers separated with just LF are separated
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/header-just-lf-separator-logs.txt");
?>
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc")) {
	echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php

include __DIR__ . "/check_headers.inc";

const LOGS_PATH = __DIR__ . "/header-just-lf-separator-logs.txt";

// Headers reach sdl_set_uri_credentials() as a string
$context = stream_context_create([
	'http' => ['header' => "Authorization: Basic\nFoo: bar"],
]);

echo "Headers are in a string:\n";
check_headers_for_import(LOGS_PATH, $context);
check_headers_for_schema(LOGS_PATH, $context);

// Headers reach sdl_set_uri_credentials() as an array
$context = stream_context_create([
	'http' => [
		// having a protocol here means that SOAP won't add it itself
		'protocol_version' => 1.1,
		'header' => "Authorization: Basic\nFoo: bar"
	],
]);

echo "Headers are in an array:\n";
check_headers_for_import(LOGS_PATH, $context);
check_headers_for_schema(LOGS_PATH, $context);

?>
--EXPECTF--
Headers are in a string:
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Foo' => 'bar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Foo' => 'bar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Foo' => 'bar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Foo' => 'bar',
)"
Headers are in an array:
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Authorization' => 'Basic',
  'Foo' => 'bar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Authorization' => 'Basic',
  'Foo' => 'bar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Authorization' => 'Basic',
  'Foo' => 'bar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Authorization' => 'Basic',
  'Foo' => 'bar',
)"
