--TEST--
SOAP Interop Round4 GroupI XSD 008 (php/wsdl): echoComplexType
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPComplexType {
    function __construct(public $varString, public $varInt, public $varFloat) {}
}
$struct = new SOAPComplexType('arg',34,325.325);
$client = new SoapClient(__DIR__."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoComplexType(array("inputComplexType"=>$struct));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoComplexType><ns2:inputComplexType><ns1:varInt>34</ns1:varInt><ns1:varString>arg</ns1:varString><ns1:varFloat>325.325</ns1:varFloat></ns2:inputComplexType></ns2:echoComplexType></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoComplexTypeResponse><ns2:return><ns1:varInt>34</ns1:varInt><ns1:varString>arg</ns1:varString><ns1:varFloat>325.325</ns1:varFloat></ns2:return></ns2:echoComplexTypeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
