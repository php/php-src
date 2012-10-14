--TEST--
Bug #35273 (Error in mapping soap - java types)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestSoapClient extends SoapClient {
  function __doRequest($request, $location, $action, $version, $one_way = 0) {
  	echo $request;
  	exit;
	}
}

ini_set("soap.wsdl_cache_enabled", 0);
$client = new TestSoapClient(dirname(__FILE__).'/bug32941.wsdl', array("trace" => 1, 'exceptions' => 0));
$ahoj = $client->echoPerson(array("name"=>"Name","surname"=>"Surname"));
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://service" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="urn:service.EchoService" xmlns:xsd="http://www.w3.org/2001/XMLSchema" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoPerson><p xsi:type="ns2:Person"><name xsi:type="SOAP-ENC:string">Name</name><surname xsi:type="SOAP-ENC:string">Surname</surname></p></ns1:echoPerson></SOAP-ENV:Body></SOAP-ENV:Envelope>
