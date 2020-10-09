--TEST--
Bug #37013 (server hangs when returning circular object references)
--SKIPIF--
<?php
  if (!extension_loaded('soap')) die('skip soap extension not available');
?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$request = <<<REQUEST
<?xml version="1.0" encoding="UTF-8"?><soapenv:Envelope
xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

<soapenv:Body>
<ns2:getThingWithParent
 soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
 xmlns:ns2="urn:test.soapserver#"/>
</soapenv:Body>

</soapenv:Envelope>
REQUEST;


class ThingWithParent
{
 var $parent;
 var $id;
 var $children;
 function __construct( $id, $parent ) {
  $this->id = $id;
  $this->parent = $parent;
 }
}


class MultiRefTest {
 public function getThingWithParent() {
  $p = new ThingWithParent( 1, null );
  $p2 = new ThingWithParent( 2, $p );
  $p3 = new ThingWithParent( 3, $p );

  $p->children = array( $p2, $p3 );

  return $p2;
 }
}


$server = new SoapServer(__DIR__."/bug37013.wsdl");
$server->setClass( "MultiRefTest");
$server->handle( $request );
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test.soapserver#" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getThingWithParentResponse><result id="ref1" xsi:type="SOAP-ENC:Struct"><parent id="ref2" xsi:type="SOAP-ENC:Struct"><parent xsi:nil="true"/><id xsi:type="xsd:int">1</id><children SOAP-ENC:arrayType="SOAP-ENC:Struct[2]" xsi:type="SOAP-ENC:Array"><item href="#ref1"/><item xsi:type="SOAP-ENC:Struct"><parent href="#ref2"/><id xsi:type="xsd:int">3</id><children xsi:nil="true"/></item></children></parent><id xsi:type="xsd:int">2</id><children xsi:nil="true"/></result></ns1:getThingWithParentResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
