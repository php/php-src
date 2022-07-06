--TEST--
SOAP Interop Round2 groupB 003 (soap/direct): echo2DStringArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$param = new SoapParam(new SoapVar(array(
    new SoapVar(array(
      new SoapVar('row0col0', XSD_STRING),
      new SoapVar('row0col1', XSD_STRING),
      new SoapVar('row0col2', XSD_STRING)
    ), SOAP_ENC_ARRAY),
    new SoapVar(array(
      new SoapVar('row1col0', XSD_STRING),
      new SoapVar('row1col1', XSD_STRING),
      new SoapVar('row1col2', XSD_STRING)
    ), SOAP_ENC_ARRAY)
  ), SOAP_ENC_ARRAY, "ArrayOfString2D", "http://soapinterop.org/xsd"),"input2DStringArray");
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echo2DStringArray", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echo2DStringArray><input2DStringArray SOAP-ENC:arrayType="SOAP-ENC:Array[2]" xsi:type="ns2:ArrayOfString2D"><item SOAP-ENC:arrayType="xsd:string[3]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:string">row0col0</item><item xsi:type="xsd:string">row0col1</item><item xsi:type="xsd:string">row0col2</item></item><item SOAP-ENC:arrayType="xsd:string[3]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:string">row1col0</item><item xsi:type="xsd:string">row1col1</item><item xsi:type="xsd:string">row1col2</item></item></input2DStringArray></ns1:echo2DStringArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echo2DStringArrayResponse><return SOAP-ENC:arrayType="xsd:string[2,3]" xsi:type="ns2:ArrayOfString2D"><item xsi:type="xsd:string">row0col0</item><item xsi:type="xsd:string">row0col1</item><item xsi:type="xsd:string">row0col2</item><item xsi:type="xsd:string">row1col0</item><item xsi:type="xsd:string">row1col1</item><item xsi:type="xsd:string">row1col2</item></return></ns1:echo2DStringArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
