--TEST--
SOAP Interop Round4 GroupH Complex Doc Lit 005 (php/wsdl): echoMultipleFaults1(2)
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
class BaseStruct {
    function __construct(public $structMessage, public $shortMessage) {}
}
$s1 = new SOAPStruct('arg1',34,325.325);
$s2 = new BaseStruct(new SOAPStruct('arg2',34,325.325),12);
$client = new SoapClient(__DIR__."/round4_groupH_complex_doclit.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoMultipleFaults1(array("whichFault" => 2,
                                   "param1"     => $s1,
                                   "param2"     => $s2));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_complex_doclit.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types/requestresponse" xmlns:ns2="http://soapinterop.org/types"><SOAP-ENV:Body><ns1:echoMultipleFaults1Request><ns1:whichFault>2</ns1:whichFault><ns1:param1><ns2:varString>arg1</ns2:varString><ns2:varInt>34</ns2:varInt><ns2:varFloat>325.325</ns2:varFloat></ns1:param1><ns1:param2><ns2:structMessage><ns2:varString>arg2</ns2:varString><ns2:varInt>34</ns2:varInt><ns2:varFloat>325.325</ns2:varFloat></ns2:structMessage><ns2:shortMessage>12</ns2:shortMessage></ns1:param2></ns1:echoMultipleFaults1Request></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/types/part"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoMultipleFaults1'.</faultstring><detail><ns2:BaseStructPart><ns1:structMessage><ns1:varString>arg2</ns1:varString><ns1:varInt>34</ns1:varInt><ns1:varFloat>325.325</ns1:varFloat></ns1:structMessage><ns1:shortMessage>12</ns1:shortMessage></ns2:BaseStructPart></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
