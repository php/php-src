--TEST--
Bug #68361 Segmentation fault on SoapClient::__getTypes
--EXTENSIONS--
soap
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<definitions name="TestServer" targetNamespace="http://foo.bar/testserver" xmlns:tns="http://foo.bar/testserver" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:ns="http://foo.bar/testserver/types">
  <types>
    <xsd:schema targetNamespace="http://foo.bar/testserver/types" xmlns="http://foo.bar/testserver/types">
      <xsd:complexType name="ArrayOfEmployeeReturn">
        <xsd:complexContent>
          <xsd:restriction base="soapenc:Array">
            <xsd:attribute ref="soapenc:arrayType" arrayType="ns:Employee[]"/>
          </xsd:restriction>
        </xsd:complexContent>
      </xsd:complexType>
      <xsd:complexType name="Employee">
        <xsd:sequence>
          <xsd:element name="id" type="xsd:int"/>
          <xsd:element name="department" type="xsd:string"/>
          <xsd:element name="name" type="xsd:string"/>
          <xsd:element name="age" type="xsd:int"/>
        </xsd:sequence>
      </xsd:complexType>
      <xsd:element name="Employee" nillable="true" type="ns:Employee"/>
      <xsd:complexType name="User">
        <xsd:sequence>
          <xsd:element name="name" type="xsd:string"/>
          <xsd:element name="age" type="xsd:int"/>
        </xsd:sequence>
      </xsd:complexType>
      <xsd:element name="User" nillable="true" type="ns:User"/>
    </xsd:schema>
  </types>
  <message name="getEmployeeRequest">
    <part name="name" type="xsd:name"/>
  </message>
  <message name="getEmployeeResponse">
    <part name="employeeReturn" type="ns:ArrayOfEmployeeReturn"/>
  </message>
  <message name="getUserRequest">
    <part name="id" type="xsd:id"/>
  </message>
  <message name="getUserResponse">
    <part name="userReturn" element="ns:User"/>
  </message>
  <portType name="TestServerPortType">
    <operation name="getEmployee">
      <input message="tns:getEmployeeRequest"/>
      <output message="tns:getEmployeeResponse"/>
    </operation>
    <operation name="getUser">
      <input message="tns:getUserRequest"/>
      <output message="tns:getUserResponse"/>
    </operation>
  </portType>
  <binding name="TestServerBinding" type="tns:TestServerPortType">
    <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
    <operation name="getEmployee">
      <soap:operation soapAction="http://foo.bar/testserver/#getEmployee"/>
      <input>
        <soap:body use="literal" namespace="http://foo.bar/testserver"/>
      </input>
      <output>
        <soap:body use="literal" namespace="http://foo.bar/testserver"/>
      </output>
    </operation>
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

file_put_contents(__DIR__ . "/bug68361.xml", $xml);
$client = new SoapClient(__DIR__ . "/bug68361.xml");

$res = $client->__getTypes(); // Segmentation fault here

print_r($res);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/bug68361.xml");
?>
--EXPECT--
Array
(
    [0] => anyType ArrayOfEmployeeReturn[]
    [1] => struct Employee {
 int id;
 string department;
 string name;
 int age;
}
    [2] => struct User {
 string name;
 int age;
}
)
