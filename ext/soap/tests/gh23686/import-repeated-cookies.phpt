--TEST--
GH-23686: Regression tests for repeated cookies headers (which doesn't work) (`<import>`)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/import-repeated-cookies-logs.txt");
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

const LOGS_PATH = __DIR__ . "/import-repeated-cookies-logs.txt";

$context = stream_context_create([
	'http' => ['header' => [
		"Cookie: Foo",
		"cOOKIE: Bar",
	]],
]);

check_headers_for_import(LOGS_PATH, $context, false);

?>
--EXPECTF--
SoapFault: SOAP-ERROR: Parsing WSDL: Couldn't load from 'http://localhost:%d/index.php' : failed to load external entity "http://localhost:%d/index.php"

bool(false)
SoapFault: SOAP-ERROR: Parsing WSDL: Couldn't load from 'http://localhost:%d/index.php' : failed to load external entity "http://localhost:%d/index.php"

bool(false)
Server exited with non-zero status: 1

Warning: file_get_contents(): Failed to open stream: No such file or directory in %s on line %d
Server output:
