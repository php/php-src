--TEST--
SOAP Set-Cookie option parsing starts at wrong offset due to variable shadowing
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
    $args[] = "-c";
    $args[] = php_ini_loaded_file();
}

// A 10-char name makes the wrong offset land exactly on the value "path=/evil",
// falsely matching it as a path attribute.
$code = <<<'PHP'
header("Content-Type: text/xml");
header("Set-Cookie: sessionkey=path=/evil;domain=good.com");
echo <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test-uri">
  <SOAP-ENV:Body>
    <ns1:testResponse/>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
PHP;

php_cli_server_start($code, null, $args);

$client = new SoapClient(null, [
    'location' => 'http://' . PHP_CLI_SERVER_ADDRESS . '/test/endpoint',
    'uri' => 'test-uri',
    'trace' => true,
]);

try {
    $client->__soapCall("test", []);
} catch (SoapFault $e) {
    // Response parsing may fault, cookies are still stored
}

$cookies = $client->__getCookies();

// path should default to "/test" from the request URI, not "/evil" from the value.
echo "value: " . $cookies['sessionkey'][0] . "\n";
echo "path: " . $cookies['sessionkey'][1] . "\n";
echo "domain: " . $cookies['sessionkey'][2] . "\n";
?>
--EXPECT--
value: path=/evil
path: /test
domain: good.com
