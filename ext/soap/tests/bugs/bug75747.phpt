--TEST--
Bug #75747 Types have missing properties on SoapClient::__getTypes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$_include_xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
	<xsd:attributeGroup name="CurrencyAmountGroup">
		<xsd:attribute name="Amount" type="xsd:int"/>
		<xsd:attribute name="CurrencyCode" type="xsd:string"/>
	</xsd:attributeGroup>
	<xsd:complexType name="User">
        <xsd:attributeGroup ref="CurrencyAmountGroup" />
    </xsd:complexType>
</xsd:schema>
XML;

$_import_xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<xsd:schema targetNamespace="http://foo.bar/testserver/types" xmlns="http://foo.bar/testserver/types" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
	<xsd:include schemaLocation="bug75747include.xml" />

	<xsd:element name="User" type="User"/>
</xsd:schema>
XML;

$_wsdl = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<definitions name="TestServer" targetNamespace="http://foo.bar/testserver" xmlns:tns="http://foo.bar/testserver"
             xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
             xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:ns="http://foo.bar/testserver/types">
    <types>
        <xsd:schema>
            <xsd:import namespace="http://foo.bar/testserver/types" schemaLocation="bug75747import.xml"/>
        </xsd:schema>
    </types>
    <message name="getUserRequest">
        <part name="id" type="xsd:id"/>
    </message>
    <message name="getUserResponse">
        <part name="userReturn" element="ns:User"/>
    </message>
    <portType name="TestServerPortType">
        <operation name="getUser">
            <input message="tns:getUserRequest"/>
            <output message="tns:getUserResponse"/>
        </operation>
    </portType>
    <binding name="TestServerBinding" type="tns:TestServerPortType">
        <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
        <operation name="getUser">
            <soap:operation soapAction="http://foo.bar/testserver/#getUser"/>
            <input>
                <soap:body use="literal" namespace="http://foo.bar/testserver"/>
            </input>
            <output>
                <soap:body use="literal" namespace="http://foo.bar/testserver"/>
            </output>
        </operation>
    </binding>
    <service name="TestServerService">
        <port name="TestServerPort" binding="tns:TestServerBinding">
            <soap:address location="http://localhost/wsdl-creator/TestClass.php"/>
        </port>
    </service>
</definitions>
XML;

file_put_contents(__DIR__ . "/bug75747include.xml", $_include_xml);
file_put_contents(__DIR__ . "/bug75747import.xml", $_import_xml);
file_put_contents(__DIR__ . "/bug75747wsdl.xml", $_wsdl);
$client = new SoapClient(__DIR__ . "/bug75747wsdl.xml", ['cache_wsdl' => WSDL_CACHE_NONE]);

$res = $client->__getTypes(); // Segmentation fault here

print_r($res);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/bug75747include.xml");
unlink(__DIR__ . "/bug75747import.xml");
unlink(__DIR__ . "/bug75747wsdl.xml");
?>
--EXPECT--
Array
(
    [0] => struct User {
 int Amount;
 string CurrencyCode;
}
)