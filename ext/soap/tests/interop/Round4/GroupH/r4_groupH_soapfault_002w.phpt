--TEST--
SOAP Interop Round4 GroupH SoapFault 002 (php/wsdl): echoVersionMismatchFault(SOAP 1.2)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/round4_groupH_soapfault.wsdl",array("trace"=>1,"exceptions"=>0,"soap_version"=>SOAP_1_2));
$client->echoVersionMismatchFault();
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_soapfault.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:enc="http://www.w3.org/2003/05/soap-encoding"><env:Body><ns1:echoVersionMismatchFault env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"/></env:Body></env:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:enc="http://www.w3.org/2003/05/soap-encoding"><env:Body><ns1:echoVersionMismatchFaultResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"/></env:Body></env:Envelope>
ok
