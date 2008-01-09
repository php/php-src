--TEST--
SOAP Interop Round4 GroupI XSD 028 (php/wsdl): echoAnyType
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPComplexType {
    function SOAPComplexType($s, $i, $f) {
        $this->varString = $s;
        $this->varInt = $i;
        $this->varFloat = $f;
    }
}
$struct = new SOAPComplexType('arg',34,325.325);
$client = new SoapClient(dirname(__FILE__)."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoAnyType(array('inputAnyType'=>new SoapVar($struct,SOAP_ENC_OBJECT,"SOAPComplexType","http://soapinterop.org/xsd")));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoAnyType><ns2:inputAnyType xsi:type="ns1:SOAPComplexType"><ns1:varInt>34</ns1:varInt><ns1:varString>arg</ns1:varString><ns1:varFloat>325.325</ns1:varFloat></ns2:inputAnyType></ns2:echoAnyType></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoAnyTypeResponse><ns2:return xsi:type="ns1:SOAPComplexType"><ns1:varInt>34</ns1:varInt><ns1:varString>arg</ns1:varString><ns1:varFloat>325.325</ns1:varFloat></ns2:return></ns2:echoAnyTypeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
