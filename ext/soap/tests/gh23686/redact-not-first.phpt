--TEST--
GH-23686: Verify that a header is redacted when it isn't the first use of the name
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/redact-not-first.txt");
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

const LOGS_PATH = __DIR__ . "/redact-not-first.txt";

$context = stream_context_create([
	'http' => ['header' => [
		"X-Authorization: This should be kept",
		"Authorization: This should be removed",
		"X-Proxy-Authorization: This should also be kept",
		"Proxy-Authorization: This should also be removed",
		"X-Cookie: Last one to keep",
		"Cookie: Last one to remove",
	]],
]);

check_headers_for_import(LOGS_PATH, $context);
check_headers_for_schema(LOGS_PATH, $context);

?>
--EXPECTF--
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-Authorization' => 'This should be kept',
  'X-Proxy-Authorization' => 'This should also be kept',
  'X-Cookie' => 'Last one to keep',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-Authorization' => 'This should be kept',
  'X-Proxy-Authorization' => 'This should also be kept',
  'X-Cookie' => 'Last one to keep',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-Authorization' => 'This should be kept',
  'X-Proxy-Authorization' => 'This should also be kept',
  'X-Cookie' => 'Last one to keep',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-Authorization' => 'This should be kept',
  'X-Proxy-Authorization' => 'This should also be kept',
  'X-Cookie' => 'Last one to keep',
)"
