--TEST--
Bug #51561 (SoapServer with a extended class and using sessions, lost the setPersistence())
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
$code = "session_start();" .
        "require_once '" . __DIR__ . "/bug51561.inc';" .
        <<<'PHP'
        class Server extends Server2 {
            private $value;
            public function setValue($param) { $this->value = $param; }
            public function getValue() { return $this->value; }
        }
        $server = new SoapServer(null, array('uri' => "blablabla.com",'encoding' => "ISO-8859-1",'soap_version' => SOAP_1_2));
        $server->setClass("Server");
        $server->setPersistence(SOAP_PERSISTENCE_SESSION);
        $server->handle();
        PHP;

php_cli_server_start($code, null, $args);

$cli = new SoapClient(null, array('location' => "http://".PHP_CLI_SERVER_ADDRESS, 'uri' => "blablabla.com",'encoding' => "ISO-8859-1",'soap_version' => SOAP_1_2));
$cli->setValue(100);
$response = $cli->getValue();
echo "Get = ".$response;	

?>
--EXPECTF--
Fatal error: Uncaught SoapFault exception: [env:Receiver] SoapServer class was deserialized from the session prior to loading the class passed to SoapServer::setClass(). Start the session after loading all classes to resolve this issue. in %s:%d
Stack trace:
#0 %s(%d): SoapClient->__call('getValue', Array)
#1 {main}
  thrown in %s on line %d
