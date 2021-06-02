--TEST--
Bug #71711: Soap Server Member variables reference bug
--EXTENSIONS--
soap
--FILE--
<?php

$client = new class(null, [ 'location' => '', 'uri' => 'http://example.org']) extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
        echo $request;
        return '';
    }
};
$ref = array("foo");
$data = new stdClass;
$data->prop =& $ref;
$client->foo($data);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://example.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:foo><param0 xsi:type="SOAP-ENC:Struct"><prop SOAP-ENC:arrayType="xsd:string[1]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:string">foo</item></prop></param0></ns1:foo></SOAP-ENV:Body></SOAP-ENV:Envelope>
