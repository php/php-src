--TEST--
GH-24315 (SoapClient follows redirects to a different origin)
--EXTENSIONS--
soap
openssl
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
if ($_SERVER["REQUEST_URI"] === "/ok") {
    header("Content-Type: text/xml; charset=utf-8");
    echo '<?xml version="1.0" encoding="UTF-8"?>',
        '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">',
        '<SOAP-ENV:Body><fooResponse><result>ok</result></fooResponse></SOAP-ENV:Body>',
        '</SOAP-ENV:Envelope>';
} else {
    http_response_code(302);
    header("Location: " . base64_decode(substr($_SERVER["REQUEST_URI"], 1)));
}
PHP;

php_cli_server_start($code, null, $args);

$targets = [
    "different host" => "http://127.0.0.1:" . PHP_CLI_SERVER_PORT . "/ok",
    "different port" => "http://" . PHP_CLI_SERVER_HOSTNAME . ":" . (PHP_CLI_SERVER_PORT + 1) . "/ok",
    "protocol-relative different host" => "//127.0.0.1:" . PHP_CLI_SERVER_PORT . "/ok",
    "https upgrade to a non-default port" => "https://" . PHP_CLI_SERVER_ADDRESS . "/ok",
    "same origin" => "http://" . PHP_CLI_SERVER_ADDRESS . "/ok",
    "same origin, host case differs" => "http://" . strtoupper(PHP_CLI_SERVER_HOSTNAME) . ":" . PHP_CLI_SERVER_PORT . "/ok",
    "https upgrade on the same host" => "https://" . PHP_CLI_SERVER_HOSTNAME . "/ok",
];

$targets["different host, follow_cross_origin_redirects"] = $targets["different host"];

foreach ($targets as $label => $target) {
    $client = new SoapClient(null, [
        'location' => 'http://' . PHP_CLI_SERVER_ADDRESS . '/' . base64_encode($target),
        'uri' => 'test-uri',
        'login' => 'user',
        'password' => 'secret',
        'follow_cross_origin_redirects' => str_ends_with($label, 'follow_cross_origin_redirects'),
    ]);

    $rejected = false;
    try {
        $client->__soapCall("foo", []);
    } catch (SoapFault $e) {
        $rejected = $e->getMessage() === "Redirection to a different origin is not allowed";
    }
    echo $label, ": ", $rejected ? "rejected" : "not rejected", "\n";
}
?>
--EXPECT--
different host: rejected
different port: rejected
protocol-relative different host: rejected
https upgrade to a non-default port: rejected
same origin: not rejected
same origin, host case differs: not rejected
https upgrade on the same host: not rejected
different host, follow_cross_origin_redirects: not rejected
