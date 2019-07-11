--TEST--
SOAP customized Content-Type, eg. SwA use case
--SKIPIF--
<?php
	require_once('skipif.inc');

	if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
		echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
	}
?>
--CONFLICTS--
server
--FILE--
<?php

include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = substr(PHP_OS, 0, 3) == 'WIN'
    ? ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=php_soap.dll"] : [];
$code = <<<'PHP'
/* Receive */
$content = trim(file_get_contents("php://input")) . PHP_EOL;
PHP;

php_cli_server_start($code, null, $args);

$client = new soapclient(NULL, [
  'location' => 'http://' . PHP_CLI_SERVER_ADDRESS,
  'uri' => 'misc-uri',
  'soap_version' => SOAP_1_2,
  'user_agent' => 'Vincent JARDIN, test headers',
  'trace' => true, /* record the headers before sending */
  'stream_context' => stream_context_create([
    'http' => [
      'header' => sprintf("MIME-Version: 1.0\r\n"),
      'content_type' => sprintf("Multipart/Related")
    ],
  ]),
]);

$client->__soapCall("foo", [ 'arg1' => "XXXbar"]);

$headers = $client->__getLastRequestHeaders();

if (strpos($headers, 'Multipart/Related; action="misc-uri#foo"') === FALSE)
  printf("Content-Type NOK %s" . PHP_EOL, $headers);
else
  printf("Content-Type OK" . PHP_EOL);

/*
 * In case of an empty content-type, let's fallback to the default content.
 */
$client2 = new soapclient(NULL, [
  'location' => 'http://' . PHP_CLI_SERVER_ADDRESS,
  'uri' => 'misc-uri',
  'soap_version' => SOAP_1_2,
  'user_agent' => 'Vincent JARDIN, test headers',
  'trace' => true, /* record the headers before sending */
  'stream_context' => stream_context_create([
    'http' => [
      'header' => sprintf("MIME-Version: 1.0\r\n"),
      'content_type' => sprintf("")
    ],
  ]),
]);

$client2->__soapCall("foo", [ 'arg1' => "XXXbar"]);

$headers = $client2->__getLastRequestHeaders();

if (strpos($headers, 'Content-Type: application/soap+xml; charset=utf-8; action="misc-uri#foo"') === FALSE)
  printf("Content-Type Default NOK %s" . PHP_EOL, $headers);
else
  printf("Content-Type Default OK" . PHP_EOL);
?>
==DONE==
--EXPECT--
Content-Type OK
Content-Type Default OK
==DONE==
