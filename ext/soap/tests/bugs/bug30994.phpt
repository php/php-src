--TEST--
Bug #30994 (SOAP server unable to handle request with references)
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/"
    xmlns:tns="http://spock/kunta/kunta"
    xmlns:types="http://spock/kunta/kunta/encodedTypes"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema">

<soap:Body
soap:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
    <q1:bassCall xmlns:q1="http://spock/bass/types/kunta">
        <system xsi:type="xsd:string">XXX</system>
        <function xsi:type="xsd:string">TASKTEST</function>
        <parameter href="#id1" />
    </q1:bassCall>

    <soapenc:Array id="id1" soapenc:arrayType="tns:Item[1]">
        <Item href="#id2" />
    </soapenc:Array>

    <tns:Item id="id2" xsi:type="tns:Item">
        <key xsi:type="xsd:string">ABCabc123</key>
        <val xsi:type="xsd:string">123456</val>
    </tns:Item>

</soap:Body>
</soap:Envelope>
EOF;

function bassCall() {
  return "ok";
}

$x = new SoapServer(NULL, array("uri"=>"http://spock/kunta/kunta"));
$x->addFunction("bassCall");
$x->handle($HTTP_RAW_POST_DATA);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://spock/kunta/kunta" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:bassCallResponse><return xsi:type="xsd:string">ok</return></ns1:bassCallResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
