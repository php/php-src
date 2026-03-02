--TEST--
SOAP Interop Round2 base 011 (soap/direct): echoIntegerArray
--EXTENSIONS--
soap
--FILE--
<?php
$param =  new SoapParam(new SoapVar(array(
    new SoapVar(1,XSD_INT),
    new SoapVar(234324324,XSD_INT),
    new SoapVar(2,XSD_INT)
  ), SOAP_ENC_ARRAY, "ArrayOfint","http://soapinterop.org/xsd"), "inputIntegerArray");
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoIntegerArray", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoIntegerArray><inputIntegerArray SOAP-ENC:arrayType="xsd:int[3]" xsi:type="ns2:ArrayOfint"><item xsi:type="xsd:int">1</item><item xsi:type="xsd:int">234324324</item><item xsi:type="xsd:int">2</item></inputIntegerArray></ns1:echoIntegerArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoIntegerArrayResponse><outputIntegerArray SOAP-ENC:arrayType="xsd:int[3]" xsi:type="ns2:ArrayOfint"><item xsi:type="xsd:int">1</item><item xsi:type="xsd:int">234324324</item><item xsi:type="xsd:int">2</item></outputIntegerArray></ns1:echoIntegerArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
