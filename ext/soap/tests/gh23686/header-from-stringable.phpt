--TEST--
GH-23686: Confirm that headers set with stringable objects are ignored
--DESCRIPTION--
If these objects were converted to strings and the headers used, then they might
need to be redacted, but php_stream_url_wrap_http_ex() only cares about strings
and string elements in arrays as of PHP 8.6. This test means that if that were
to ever change, this test would fail and developers would know to update
the redaction logic in the soap extension.
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CLEAN--
<?php
@unlink(__DIR__ . "/header-from-stringable-logs.txt");
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

const LOGS_PATH = __DIR__ . "/header-from-stringable-logs.txt";

class StringWrapper {
  public function __construct(private readonly string $val) {}
  public function __toString() { return $this->val; }
}

// Headers reach sdl_set_uri_credentials() as a string
$context = stream_context_create([
	'http' => ['header' => [
    new StringWrapper("Authorization: Foo"),
    new StringWrapper("Bar: Baz"),
    "X-non-object: value",
	]],
]);

echo "Headers are in a string:\n";
check_headers_for_import(LOGS_PATH, $context);
check_headers_for_schema(LOGS_PATH, $context);

// Headers reach sdl_set_uri_credentials() as an array
$context = stream_context_create([
  'http' => [
    // having a protocol here means that SOAP won't add it itself
    'protocol_version' => 1.1,
    'header' => [
      new StringWrapper("Authorization: Foo"),
      new StringWrapper("Bar: Baz"),
      "X-non-object: value",
    ]
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
  'X-non-object' => 'value',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
Headers are in an array:
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
string(%d) "array (
  'Host' => 'localhost:%d',
  'Connection' => 'close',
  'X-non-object' => 'value',
)"
