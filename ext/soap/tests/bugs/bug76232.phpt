--TEST--
Bug #76232 (SoapClient Cookie Header Semicolon)
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
]);

echo "=== Request with one cookie ===\n";

$client->__setCookie('testcookie1', 'true');
$client->__soapCall("foo", []);
echo $client->__getLastRequestHeaders();

echo "=== Request with two cookies ===\n";

$client->__setCookie('testcookie2', 'true');
$client->__soapCall("foo", []);

echo $client->__getLastRequestHeaders();
?>
--EXPECTF--
=== Request with one cookie ===
POST / HTTP/1.1
Host: %s
Connection: Keep-Alive
User-Agent: PHP-SOAP/%s
Content-Type: text/xml; charset=utf-8
SOAPAction: "misc-uri#foo"
Content-Length: %d
Cookie: testcookie1=true

=== Request with two cookies ===
POST / HTTP/1.1
Host: %s
Connection: Keep-Alive
User-Agent: PHP-SOAP/%s
Content-Type: text/xml; charset=utf-8
SOAPAction: "misc-uri#foo"
Content-Length: %d
Cookie: testcookie1=true; testcookie2=true
