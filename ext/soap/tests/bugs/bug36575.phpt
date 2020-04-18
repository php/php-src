--TEST--
Bug #36575 (Incorrect complex type instantiation with hierarchies)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
abstract class CT_A1 {
    public $var1;
}

class CT_A2 extends CT_A1 {
    public $var2;
}

class CT_A3 extends CT_A2 {
    public $var3;
}

// returns A2 in WSDL
function test( $a1 ) {
    $a3 = new CT_A3();
    $a3->var1 = $a1->var1;
    $a3->var2 = "var two";
    $a3->var3 = "var three";
    return $a3;
}

$classMap = array("A1" => "CT_A1", "A2" => "CT_A2", "A3" => "CT_A3");

$client = new SoapClient(__DIR__."/bug36575.wsdl", array("trace" => 1, "exceptions" => 0, "classmap" => $classMap));
$a2 = new CT_A2();
$a2->var1 = "one";
$a2->var2 = "two";
$client->test($a2);

$soapRequest = $client->__getLastRequest();

echo $soapRequest;

$server = new SoapServer(__DIR__."/bug36575.wsdl", array("classmap" => $classMap));
$server->addFunction("test");
$server->handle($soapRequest);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test.soap#" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="urn:test.soap.types#" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><a1 xsi:type="ns2:A2"><var1 xsi:type="xsd:string">one</var1><var2 xsi:type="xsd:string">two</var2></a1></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test.soap#" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="urn:test.soap.types#" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><result xsi:type="ns2:A3"><var1 xsi:type="xsd:string">one</var1><var2 xsi:type="xsd:string">var two</var2><var3 xsi:type="xsd:string">var three</var3></result></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
