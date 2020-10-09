--TEST--
SOAP Interop Round4 GroupI XSD 035 (php/wsdl): echoVoidSoapHeader(6)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
$hdr = new SoapHeader("http://soapinterop.org/","echoMeComplexTypeRequest", array("varInt"=>34,"varString"=>"arg","varFloat"=>12.345), 1, SOAP_ACTOR_NEXT);
$client = new SoapClient(__DIR__."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->__soapCall("echoVoidSoapHeader",array(),null,$hdr);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org" xmlns:ns2="http://soapinterop.org/echoheader/" xmlns:ns3="http://soapinterop.org/"><SOAP-ENV:Header><ns3:echoMeComplexTypeRequest SOAP-ENV:mustUnderstand="1" SOAP-ENV:actor="http://schemas.xmlsoap.org/soap/actor/next"><ns2:varString>arg</ns2:varString><ns2:varInt>34</ns2:varInt><ns2:varFloat>12.345</ns2:varFloat></ns3:echoMeComplexTypeRequest></SOAP-ENV:Header><SOAP-ENV:Body><ns1:echoVoidSoapHeader/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/echoheader/" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Header><ns2:echoMeComplexTypeResponse><ns1:varString>arg</ns1:varString><ns1:varInt>34</ns1:varInt><ns1:varFloat>12.345</ns1:varFloat></ns2:echoMeComplexTypeResponse></SOAP-ENV:Header><SOAP-ENV:Body><ns2:echoVoidSoapHeaderResponse/></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
