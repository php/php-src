--TEST--
SoapClient must truncate a soapaction that contains newline characters
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

foreach ([SOAP_1_1, SOAP_1_2] as $version) {
  $client = new SoapClient(NULL, [
    'location' => 'http://' . PHP_CLI_SERVER_ADDRESS,
    'uri' => 'misc-uri',
    'trace' => true,
    'soap_version' => $version,
  ]);

  $client->__soapCall("foo", [], ['soapaction' => "an-action\r\nX-Injected: yes"]);
  echo $client->__getLastRequestHeaders();
}

?>
--EXPECTF--
Warning: SoapClient::__doRequest(): Header SOAPAction value contains newline characters and has been truncated in %s on line %d
POST / HTTP/1.1
Host: localhost:%d
Connection: Keep-Alive
User-Agent: PHP-SOAP/%s
Content-Type: text/xml; charset=utf-8
SOAPAction: "an-action"
Content-Length: %d


Warning: SoapClient::__doRequest(): Header SOAPAction value contains newline characters and has been truncated in %s on line %d
POST / HTTP/1.1
Host: localhost:%d
Connection: Keep-Alive
User-Agent: PHP-SOAP/%s
Content-Type: application/soap+xml; charset=utf-8; action="an-action"
Content-Length: %d
