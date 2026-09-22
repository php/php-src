--TEST--
GHSA-85c2-q967-79q5: Stale SOAP_GLOBAL(ref_map) pointer with Apache Map
--CREDITS--
brettgervasoni
--EXTENSIONS--
soap
--FILE--
<?php

class Handler {
    public function test(...$args) {
        $GLOBALS['result'] = $args;
    }
}

$envelope = <<<'XML'
<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope
    xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema">

    <soapenv:Body>
        <test>
            <map xsi:type="apache:Map" xmlns:apache="http://xml.apache.org/xml-soap">
                <item>
                    <key>foo</key>
                    <value id="stale"><object>bar</object></value>
                </item>
                <item>
                    <key>foo</key>
                    <value>baz</value>
                </item>
            </map>
            <stale href="#stale"/>
        </test>
    </soapenv:Body>
</soapenv:Envelope>
XML;

$s = new SoapServer(null, ['uri' => 'urn:a']);
$s->setClass(Handler::class);
$s->handle($envelope);
var_dump($result);

?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:a" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return xsi:nil="true"/></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  [0]=>
  array(1) {
    ["foo"]=>
    string(3) "baz"
  }
  [1]=>
  object(stdClass)#%d (1) {
    ["object"]=>
    string(3) "bar"
  }
}
