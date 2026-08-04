--TEST--
SOAP client follows a redirect with a relative Location resolved against the request path
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

$code = <<<'PHP'
if ($_SERVER["REQUEST_URI"] === "/svc/redirected") {
    header("Content-Type: text/xml; charset=utf-8");
    echo '<?xml version="1.0" encoding="UTF-8"?>',
        '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">',
        '<SOAP-ENV:Body><fooResponse><result>ok</result></fooResponse></SOAP-ENV:Body>',
        '</SOAP-ENV:Envelope>';
} else {
    http_response_code(302);
    header("Location: redirected");
}
PHP;

php_cli_server_start($code, null, $args);

$client = new SoapClient(null, [
    'location' => 'http://' . PHP_CLI_SERVER_ADDRESS . '/svc/start',
    'uri' => 'test-uri',
]);

try {
    $client->__soapCall("foo", []);
    echo "redirect followed\n";
} catch (SoapFault $e) {
    echo "SoapFault: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
redirect followed
