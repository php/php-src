--TEST--
Bug #42183 (classmap cause crash in non-wsdl mode )
--EXTENSIONS--
soap
--FILE--
<?php
class PHPObject {
}

$req = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://ws.sit.com" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;

function test() {
    return new PHPObject();
}

$server = new SoapServer(NULL, array('uri' => 'http://ws.sit.com',
    'classmap' => array('Object' => 'PHPObject')));
$server->addFunction("test");
ob_start();
$server->handle($req);
ob_end_clean();
echo "ok\n";
?>
--EXPECT--
ok
