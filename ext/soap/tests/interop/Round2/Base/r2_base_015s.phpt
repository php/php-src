--TEST--
SOAP Interop Round2 base 015 (soap/direct): echoStructArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
--FILE--
<?php
class SOAPStruct {
    function SOAPStruct($s, $i, $f) {
        $this->varString = $s;
        $this->varInt = $i;
        $this->varFloat = $f;
    }
}

$struct1 = new SoapVar(array(
		new SoapVar('arg', XSD_STRING, null, null, 'varString'),
		new SoapVar('34',  XSD_INT, null, null, 'varInt'),
		new SoapVar('325.325',  XSD_FLOAT, null, null, 'varFloat')
  ),SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd");
$struct2 = new SoapVar(array(
		new SoapVar('arg', XSD_STRING, null, null, 'varString'),
		new SoapVar('34',  XSD_INT, null, null, 'varInt'),
		new SoapVar('325.325',  XSD_FLOAT, null, null, 'varFloat')
  ),SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd");

$param =   new SoapParam(new SoapVar(array(
    $struct1,
    $struct2
  ),SOAP_ENC_ARRAY,"ArrayOfSOAPStruct","http://soapinterop.org/xsd"), "inputStructArray");
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoStructArray", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructArray><inputStructArray SOAP-ENC:arrayType="ns2:SOAPStruct[2]" xsi:type="ns2:ArrayOfSOAPStruct"><item xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat></item><item xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat></item></inputStructArray></ns1:echoStructArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructArrayResponse><outputStructArray SOAP-ENC:arrayType="ns2:SOAPStruct[2]" xsi:type="ns2:ArrayOfSOAPStruct"><item xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat></item><item xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat></item></outputStructArray></ns1:echoStructArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
