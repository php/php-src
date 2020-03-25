--TEST--
SOAP Interop Round2 groupB 003 (php/wsdl): echo2DStringArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$param = array(
    array('row0col0', 'row0col1', 'row0col2'),
    array('row1col0', 'row1col1', 'row1col2'));
$client = new SoapClient(__DIR__."/round2_groupB.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echo2DStringArray($param);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echo2DStringArray><input2DStringArray SOAP-ENC:arrayType="xsd:string[2,3]" xsi:type="ns2:ArrayOfString2D"><item xsi:type="xsd:string">row0col0</item><item xsi:type="xsd:string">row0col1</item><item xsi:type="xsd:string">row0col2</item><item xsi:type="xsd:string">row1col0</item><item xsi:type="xsd:string">row1col1</item><item xsi:type="xsd:string">row1col2</item></input2DStringArray></ns1:echo2DStringArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echo2DStringArrayResponse><return SOAP-ENC:arrayType="xsd:string[2,3]" xsi:type="ns2:ArrayOfString2D"><item xsi:type="xsd:string">row0col0</item><item xsi:type="xsd:string">row0col1</item><item xsi:type="xsd:string">row0col2</item><item xsi:type="xsd:string">row1col0</item><item xsi:type="xsd:string">row1col1</item><item xsi:type="xsd:string">row1col2</item></return></ns1:echo2DStringArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
