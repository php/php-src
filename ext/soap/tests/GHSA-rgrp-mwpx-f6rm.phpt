--TEST--
GHSA-rgrp-mwpx-f6rm: Stack overflow on deeply nested XML
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

function envelope(int $depth): string
{
    return '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><test>'
        . str_repeat('<a>', $depth) . 'leaf' . str_repeat('</a>', $depth)
        . '</test></s:Body></s:Envelope>';
}

function test($arg)
{
    $depth = 0;
    while (is_object($arg)) {
        $depth++;
        $arg = $arg->a;
    }
    $GLOBALS['decoded'] = [$depth, $arg];

    return 'ok';
}

$server = new SoapServer(null, ['uri' => 'urn:test']);
$server->addFunction('test');

$server->handle(envelope(200));
var_dump($GLOBALS['decoded']);

/* libxml2 2.13 rejects past 2048 despite XML_PARSE_HUGE, older ones do not. */
$wsdl = __DIR__ . '/GHSA-rgrp-mwpx-f6rm.wsdl';
foreach ([1000, 2100] as $depth) {
    file_put_contents($wsdl, '<r>' . str_repeat('<a>', $depth) . 'leaf' . str_repeat('</a>', $depth) . '</r>');
    try {
        new SoapClient($wsdl);
    } catch (SoapFault $e) {
        echo rtrim($e->getMessage()), "\n";
    }
}

/* A WSDL reached through an entity reference still loads. Its replacement tree cannot
   be built deeper than the 2048 libxml2 2.13 enforces, so this pins the entity skip
   rather than the limit. */
$entity = __DIR__ . '/GHSA-rgrp-mwpx-f6rm-entity.wsdl';
$desc = str_repeat('&#60;p&#62;', 300) . 'A test service.' . str_repeat('&#60;/p&#62;', 300);
file_put_contents($entity, '<?xml version="1.0"?>
<!DOCTYPE definitions [<!ENTITY desc "' . $desc . '">]>
<definitions xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:tns="urn:test" targetNamespace="urn:test" name="T">
  <documentation>&desc;</documentation>
  <message name="inMsg"/>
  <portType name="pt"><operation name="op"><input message="tns:inMsg"/></operation></portType>
  <binding name="b" type="tns:pt">
    <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
    <operation name="op"><soap:operation soapAction="op"/><input><soap:body use="encoded" namespace="urn:test" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/></input></operation>
  </binding>
  <service name="s"><port name="p" binding="tns:b"><soap:address location="http://example.org/"/></port></service>
</definitions>');
var_dump((new SoapClient($entity))->__getFunctions());

$server->handle(envelope(3000));

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/GHSA-rgrp-mwpx-f6rm.wsdl');
@unlink(__DIR__ . '/GHSA-rgrp-mwpx-f6rm-entity.wsdl');
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return xsi:type="xsd:string">ok</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  [0]=>
  int(199)
  [1]=>
  string(4) "leaf"
}
SOAP-ERROR: Parsing WSDL: Couldn't find <definitions> in '%sGHSA-rgrp-mwpx-f6rm.wsdl'
SOAP-ERROR: Parsing WSDL: Couldn't load from '%sGHSA-rgrp-mwpx-f6rm.wsdl'%S
array(1) {
  [0]=>
  string(9) "void op()"
}
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>Bad Request</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
