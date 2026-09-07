--TEST--
SoapClient must truncate a cookie name or value that contains newline characters
--EXTENSIONS--
soap
--SKIPIF--
<?php
    if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
        echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
    }
?>
--FILE--
<?php

include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "soap." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
  $args[] = "-c";
  $args[] = php_ini_loaded_file();
}
$code = <<<'PHP'
$content = trim(file_get_contents("php://input")) . PHP_EOL;
PHP;

php_cli_server_start($code, null, $args);

$client = new SoapClient(NULL, [
  'location' => 'http://' . PHP_CLI_SERVER_ADDRESS,
  'uri' => 'misc-uri',
  'trace' => true,
]);

$client->__setCookie("evil-value", "a\r\nX-Injected: yes");
$client->__setCookie("evil-name\r\nX-Injected: yes", "b");
$client->__setCookie("harmless", "c");

$client->__soapCall("foo", []);
echo $client->__getLastRequestHeaders();

?>
--EXPECTF--
Warning: SoapClient::__doRequest(): Header Cookie value contains newline characters and has been truncated in %s on line %d

Warning: SoapClient::__doRequest(): Header Cookie value contains newline characters and has been truncated in %s on line %d
POST / HTTP/1.1
Host: localhost:%d
Connection: Keep-Alive
User-Agent: PHP-SOAP/%s
Content-Type: text/xml; charset=utf-8
SOAPAction: "misc-uri#foo"
Content-Length: %d
Cookie: evil-value=a; evil-name=b; harmless=c
