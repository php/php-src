--TEST--
SOAP Interop Round4 GroupH Complex RPC Enc 003 (php/wsdl): echoExtendedStructFault
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class BaseStruct {
    function __construct(public $floatMessage, public $shortMessage) {}
}
class ExtendedStruct extends BaseStruct {
    function __construct(
        $floatMessage, $shortMessage,
        public $stringMessage, public $intMessage, public $anotherIntMessage)
    {
        parent::__construct($floatMessage, $shortMessage);
    }
}
$struct = new ExtendedStruct(12.345,12,"arg",-3,5);
$client = new SoapClient(__DIR__."/round4_groupH_complex_rpcenc.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoExtendedStructFault($struct);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_complex_rpcenc.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/types" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoExtendedStructFault><param xsi:type="ns2:ExtendedStruct"><floatMessage xsi:type="xsd:float">12.345</floatMessage><shortMessage xsi:type="xsd:short">12</shortMessage><stringMessage xsi:type="xsd:string">arg</stringMessage><intMessage xsi:type="xsd:int">-3</intMessage><anotherIntMessage xsi:type="xsd:int">5</anotherIntMessage></param></ns1:echoExtendedStructFault></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/wsdl" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoExtendedStructFault'.</faultstring><detail><ns2:part3 xsi:type="ns1:ExtendedStruct"><floatMessage xsi:type="xsd:float">12.345</floatMessage><shortMessage xsi:type="xsd:short">12</shortMessage><stringMessage xsi:type="xsd:string">arg</stringMessage><intMessage xsi:type="xsd:int">-3</intMessage><anotherIntMessage xsi:type="xsd:int">5</anotherIntMessage></ns2:part3></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
