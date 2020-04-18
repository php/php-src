--TEST--
SOAP Interop Round4 GroupH Complex RPC Enc 002 (php/wsdl): echoBaseStructFault
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class BaseStruct {
    function __construct($f, $s) {
        $this->floatMessage = $f;
        $this->shortMessage = $s;
    }
}
$struct = new BaseStruct(12.345,12);
$client = new SoapClient(__DIR__."/round4_groupH_complex_rpcenc.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoBaseStructFault($struct);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_complex_rpcenc.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/types" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoBaseStructFault><param xsi:type="ns2:BaseStruct"><floatMessage xsi:type="xsd:float">12.345</floatMessage><shortMessage xsi:type="xsd:short">12</shortMessage></param></ns1:echoBaseStructFault></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/wsdl" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoBaseStructFault'.</faultstring><detail><ns2:part2 xsi:type="ns1:BaseStruct"><floatMessage xsi:type="xsd:float">12.345</floatMessage><shortMessage xsi:type="xsd:short">12</shortMessage></ns2:part2></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
