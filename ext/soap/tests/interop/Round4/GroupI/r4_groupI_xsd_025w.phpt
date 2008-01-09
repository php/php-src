--TEST--
SOAP Interop Round4 GroupI XSD 025 (php/wsdl): echoNestedMultiOccurs
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPMultiOccursComplexType {
    function SOAPMultiOccursComplexType($s, $i, $f, $c) {
        $this->varString = $s;
        $this->varInt = $i;
        $this->varFloat = $f;
        $this->varMultiOccurs = $c;
    }
}
$struct = new SOAPMultiOccursComplexType("arg",34,12.345,array("red","green","blue"));
$client = new SoapClient(dirname(__FILE__)."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoNestedMultiOccurs(array("inputComplexType"=>$struct));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoNestedMultiOccurs><ns2:inputComplexType><ns1:varString>arg</ns1:varString><ns1:varInt>34</ns1:varInt><ns1:varFloat>12.345</ns1:varFloat><ns1:varMultiOccurs><ns1:string>red</ns1:string><ns1:string>green</ns1:string><ns1:string>blue</ns1:string></ns1:varMultiOccurs></ns2:inputComplexType></ns2:echoNestedMultiOccurs></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoNestedMultiOccursResponse><ns2:return><ns1:varString>arg</ns1:varString><ns1:varInt>34</ns1:varInt><ns1:varFloat>12.345</ns1:varFloat><ns1:varMultiOccurs><ns1:string>red</ns1:string><ns1:string>green</ns1:string><ns1:string>blue</ns1:string></ns1:varMultiOccurs></ns2:return></ns2:echoNestedMultiOccursResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
