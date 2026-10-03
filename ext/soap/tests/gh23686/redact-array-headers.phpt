--TEST--
GH-23686: Verify that a header is redacted when it reaches sdl_set_uri_credentials() as an array
--DESCRIPTION--
Most of the time soap will add its own headers and in the process convert user headers
from an array to a string, check for the case when that does not happen
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/redact-array-headers.txt");
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

const LOGS_PATH = __DIR__ . "/redact-array-headers.txt";

$context = stream_context_create([
  'http' => [
    // having a protocol here means that SOAP won't add it itself
    'protocol_version' => 1.1,
    'header' => [
      "X-Authorization: This should be kept",
      "Authorization: This should be removed",
      "X-Proxy-Authorization: This should also be kept",
      "Proxy-Authorization: This should also be removed",
      "X-Cookie: Last one to keep",
      "Cookie: Last one to remove",
    ]
  ],
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
