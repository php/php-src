--TEST--
SOAP Server 12: WSDL generation
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
WSDL
--FILE--
<?php
function Add($x,$y) {
  return $x+$y;
}

$server = new soapserver("http://testuri.org");
$server->addfunction("Add");
$server->handle();
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><SOAP-ENV:Fault xsi:type="SOAP-ENC:Struct"><faultstring xsi:type="xsd:string">WSDL generation is not supported yet</faultstring><faultcode xsi:type="xsd:string">SOAP-ENV:Server</faultcode><detail xsi:nil="1"/></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
