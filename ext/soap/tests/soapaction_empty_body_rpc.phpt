--TEST--
SoapServer::handle() with empty Body and SOAPAction matching an RPC operation
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--POST--
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
    <SOAP-ENV:Body/>
</SOAP-ENV:Envelope>
--FILE--
<?php
function getItems() {
    return [];
}

$server = new SoapServer(__DIR__.'/server030.wsdl');
$server->addFunction("getItems");
$_SERVER["HTTP_SOAPACTION"] = "http://testuri.orgTestServiceAction";
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getItemsResponse><getItemsReturn SOAP-ENC:arrayType="ns1:Item[0]" xsi:type="ns1:ItemArray"/></ns1:getItemsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
