--TEST--
GH-25267 (NULL zend_string dereference in to_xml_any with integer key)
--EXTENSIONS--
soap
--CREDITS--
YuanchengJiang
--FILE--
<?php
class C extends SoapClient {
    function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        echo $request;
        return "";
    }
}
$c = new C(null, ["location" => "test://", "uri" => "test://"]);
$c->foo(new SoapVar([new SoapVar("y", XSD_STRING), "a" => new SoapVar("x", XSD_STRING)], XSD_ANYXML));
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:foo><BOGUS xsi:type="xsd:string">y</BOGUS><a xsi:type="xsd:string">x</a></ns1:foo></SOAP-ENV:Body></SOAP-ENV:Envelope>
