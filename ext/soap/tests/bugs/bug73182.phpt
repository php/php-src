--TEST--
Bug #73182 (PHP SOAPClient does not support stream context HTTP headers in array form)
--EXTENSIONS--
soap
--SKIPIF--
<?php
    if (!file_exists(__DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc")) {
        echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
    }
?>
--FILE--
<?php

include __DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc";

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "soap." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
  // Necessary such that it works from a development directory in which case extension_dir might not be the real extension dir
  $args[] = "-c";
  $args[] = php_ini_loaded_file();
}
$code = <<<'PHP'
/* Receive */
$content = trim(file_get_contents("php://input")) . PHP_EOL;
PHP;

php_cli_server_start($code, null, $args);

$client = new soapclient(NULL, [
  'location' => 'http://' . PHP_CLI_SERVER_ADDRESS,
  'uri' => 'misc-uri',
  'trace' => true,
  'stream_context' => stream_context_create([
    'http' => [
      'header' => [
        // These 2 must be ignored because the soap http client sets them up
        'Connection: close',
        'User-Agent: bar',
        // The following 2 must be included
        'X-custom: foo',
        'Content-Description: Foo',
        '  X-custom2: trim me  ',
      ],
    ],
  ]),
]);

$client->__soapCall("foo", []);
echo $client->__getLastRequestHeaders();

?>
--EXPECTF--
POST / HTTP/1.1
Host: localhost:%d
Connection: Keep-Alive
User-Agent: PHP-SOAP/%s
Content-Type: text/xml; charset=utf-8
SOAPAction: "misc-uri#foo"
Content-Length: %d
X-custom: foo
Content-Description: Foo
X-custom2: trim me
