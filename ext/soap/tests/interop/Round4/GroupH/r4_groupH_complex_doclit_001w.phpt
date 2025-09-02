--TEST--
SOAP Interop Round4 GroupH Complex Doc Lit 001 (php/wsdl): echoSOAPStructFault
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPStruct {
    function __construct(public $varString, public $varInt, public $varFloat) {}
}
$struct = new SOAPStruct('arg',34,325.325);
$client = new SoapClient(__DIR__."/round4_groupH_complex_doclit.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoSOAPStructFault($struct);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_complex_doclit.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/types/requestresponse"><SOAP-ENV:Body><ns2:echoSOAPStructFaultRequest><ns1:varString>arg</ns1:varString><ns1:varInt>34</ns1:varInt><ns1:varFloat>325.325</ns1:varFloat></ns2:echoSOAPStructFaultRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/types/part"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoSOAPStructFault'.</faultstring><detail><ns2:SOAPStructFaultPart><ns1:soapStruct><ns1:varString>arg</ns1:varString><ns1:varInt>34</ns1:varInt><ns1:varFloat>325.325</ns1:varFloat></ns1:soapStruct></ns2:SOAPStructFaultPart></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
