--TEST--
SOAP Interop Round3 GroupF Headers 004 (php/wsdl): echoString
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$hdr = array(
  new SoapHeader("http://soapinterop.org/xsd","Header1", array("int"=>34,"string"=>"arg1")),
  new SoapHeader("http://soapinterop.org/xsd","Header2", array("int"=>43,"string"=>"arg2"))
);
$client = new SoapClient(dirname(__FILE__)."/round3_groupF_headers.wsdl",array("trace"=>1,"exceptions"=>0));
$client->__soapCall("echoString",array("Hello World"),null,$hdr);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupF_headers.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Header><ns1:Header1><ns1:string>arg1</ns1:string><ns1:int>34</ns1:int></ns1:Header1><ns1:Header2><ns1:int>43</ns1:int><ns1:string>arg2</ns1:string></ns1:Header2></SOAP-ENV:Header><SOAP-ENV:Body><ns1:echoStringParam>Hello World</ns1:echoStringParam></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStringReturn>Hello World</ns1:echoStringReturn></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
