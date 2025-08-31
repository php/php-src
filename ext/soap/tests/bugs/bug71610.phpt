--TEST--
SOAP Bug #71610 - Type Confusion Vulnerability - SOAP / make_http_soap_request()
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
php_cli_server_start();

$url = "http://" . PHP_CLI_SERVER_ADDRESS;
$ser = 'O:10:"SoapClient":3:{s:3:"uri";s:1:"a";s:8:"location";s:' . strlen($url) . ':"'
    . $url . '";s:8:"_cookies";a:1:{s:8:"manhluat";a:3:{i:0;s:0:"";i:1;N;i:2;N;}}}';

$exploit = unserialize($ser);
try {
$exploit->blahblah();
} catch(SoapFault $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECT--
looks like we got no XML document
