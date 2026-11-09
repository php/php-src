--TEST--
GH-25303 (NULL encode dereference in to_xml_list() error path for union types)
--EXTENSIONS--
soap
--CREDITS--
YuanchengJiang
--FILE--
<?php
$wsdl = <<<XML
<definitions xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:tns="urn:t" targetNamespace="urn:t">
  <types>
    <xsd:schema targetNamespace="urn:t">
      <xsd:simpleType name="U"><xsd:union memberTypes="xsd:int"/></xsd:simpleType>
    </xsd:schema>
  </types>
  <message name="m"><part name="p" type="tns:U"/></message>
  <portType name="pt"><operation name="test"><input message="tns:m"/></operation></portType>
  <binding name="b" type="tns:pt">
    <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
    <operation name="test"><soap:operation soapAction="#test"/>
      <input><soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/></input>
    </operation>
  </binding>
  <service name="s"><port name="p" binding="tns:b"><soap:address location="test://"/></port></service>
</definitions>
XML;
$c = new SoapClient("data://text/plain;base64," . base64_encode($wsdl), ["cache_wsdl" => WSDL_CACHE_NONE]);
try {
    $c->test([[1]]);
} catch (SoapFault $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
SOAP-ERROR: Encoding: Value does not match any member type of union type 'U'
