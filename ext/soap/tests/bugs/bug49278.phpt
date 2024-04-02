--TEST--
Bug #49278 (SoapClient::__getLastResponseHeaders returns NULL if wsdl operation !has output)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
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
$code = '$server = new SoapServer("' . __DIR__ . '"/bug49278.wsdl", ["trace"=>1]);' .
        <<<'PHP'
        function Add() {}
        $server->addFunction('Add');
        $server->handle();
        PHP;

php_cli_server_start($code, null, $args);

$client = new SoapClient(__DIR__ . '/bug49278.wsdl', array('location' => "http://".PHP_CLI_SERVER_ADDRESS, 'trace' => 1));
$client->Add();
var_dump($client->__getLastResponse());
var_dump($client->__getLastResponse());
var_dump($client->__getLastRequestHeaders());
var_dump($client->__getLastResponseHeaders());

?>
--EXPECTF--
string(0) ""
string(0) ""
string(177) "POST / HTTP/1.1
Host: %s
Connection: Keep-Alive
User-Agent: PHP-SOAP/8.4.0-dev
Content-Type: text/xml; charset=utf-8
SOAPAction: "Add"
Content-Length: %d

"
string(165) "HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/8.4.0-dev
Content-type: text/html; charset=UTF-8
"
