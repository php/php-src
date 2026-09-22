--TEST--
GH-16259 (Soap segfault when classmap instantiation fails)
--EXTENSIONS--
soap
--FILE--
<?php
abstract class CT_A1 {
}
class CT_A2 extends CT_A1 {
}

$classMap = array("A1" => "CT_A1", "A2" => "CT_A2");
$client = new SoapClient(__DIR__."/bug36575.wsdl", array("trace" => 1, "exceptions" => 0));
$a2 = new CT_A2();
$client->test($a2);
$soapRequest = $client->__getLastRequest();

$server = new SoapServer(__DIR__."/bug36575.wsdl", array("classmap" => $classMap));
$server->handle($soapRequest);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Cannot instantiate abstract class CT_A1</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
