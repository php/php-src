--TEST--
GH-23686: Regression tests for repeated cookies headers (which previously didn't work) (`<schema>`)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/schema-repeated-cookies-logs.txt");
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

const LOGS_PATH = __DIR__ . "/schema-repeated-cookies-logs.txt";

$context = stream_context_create([
	'http' => ['header' => [
		"Cookie: Foo",
		"cOOKIE: Bar",
	]],
]);

check_headers_for_schema(LOGS_PATH, $context, [], false);

?>
--EXPECTF--
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
)"
