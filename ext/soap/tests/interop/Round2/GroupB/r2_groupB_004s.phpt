--TEST--
SOAP Interop Round2 groupB 004 (soap/direct): echoNestedStruct
--EXTENSIONS--
soap
--INI--
precision=14
--FILE--
<?php
$param = new SoapParam(new SoapVar(array(
    new SoapVar("arg", XSD_STRING, null, null, "varString"),
    new SoapVar(34, XSD_INT, null, null, "varInt"),
    new SoapVar(123.45, XSD_FLOAT, null, null, "varFloat"),
    new SoapVar(array(
        new SoapVar("arg2", XSD_STRING, null, null, "varString"),
      new SoapVar(342, XSD_INT, null, null, "varInt"),
      new SoapVar(123.452, XSD_FLOAT, null, null, "varFloat")
    ), SOAP_ENC_OBJECT, "SOAPStruct", "http://soapinterop.org/xsd", 'varStruct')
  ), SOAP_ENC_OBJECT, "SOAPStructStruct", "http://soapinterop.org/xsd"), "inputStruct");
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoNestedStruct", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoNestedStruct><inputStruct xsi:type="ns2:SOAPStructStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">123.45</varFloat><varStruct xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg2</varString><varInt xsi:type="xsd:int">342</varInt><varFloat xsi:type="xsd:float">123.452</varFloat></varStruct></inputStruct></ns1:echoNestedStruct></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoNestedStructResponse><return xsi:type="ns2:SOAPStructStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">123.45</varFloat><varStruct xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg2</varString><varInt xsi:type="xsd:int">342</varInt><varFloat xsi:type="xsd:float">123.452</varFloat></varStruct></return></ns1:echoNestedStructResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
