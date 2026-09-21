--TEST--
GH-23686: Regression test for `Authorization: Bearer`  (`<import>`)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/import-authorization-bearer-logs.txt");
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

const LOGS_PATH = __DIR__ . "/import-authorization-bearer-logs.txt";

$context = stream_context_create([
	'http' => ['header' => "Authorization: Bearer secret-token"],
]);

check_headers_for_import(LOGS_PATH, $context);

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
