--TEST--
GH-23686: `Proxy-Authorization` header kept but others removed (`<import>`)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
unlink(__DIR__ . "/import-keep-proxy-authorization-logs.txt");
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

const LOGS_PATH = __DIR__ . "/import-keep-proxy-authorization-logs.txt";

$context = stream_context_create([
	'http' => ['header' => "Authorization: Bearer secret-token\r\nProxy-Authorization: FooBar\r\nCookie: foo=bar"],
]);

check_headers_for_import(LOGS_PATH, $context, ["keep_headers" => ["proxy-authorization"]]);

?>
--EXPECTF--
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Proxy-Authorization' => 'FooBar',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'Proxy-Authorization' => 'FooBar',
)"
