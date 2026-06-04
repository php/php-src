--TEST--
GH-22167 (Out-of-range XML Schema integer values in SOAP WSDL)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function wsdl_with_schema(string $schema): string {
    return <<<XML
<?xml version="1.0"?>
<definitions
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:tns="http://test-uri/"
    xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
    xmlns="http://schemas.xmlsoap.org/wsdl/"
    targetNamespace="http://test-uri/">
  <types>
    <xsd:schema targetNamespace="http://test-uri/">
      $schema
    </xsd:schema>
  </types>
  <message name="m"><part name="p" type="tns:T"/></message>
  <portType name="p"><operation name="op"><input message="tns:m"/></operation></portType>
  <binding name="b" type="tns:p">
    <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
    <operation name="op">
      <soap:operation soapAction="#op"/>
      <input>
        <soap:body use="encoded"
                   namespace="http://test-uri/"
                   encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
    </operation>
  </binding>
  <service name="s"><port name="p" binding="tns:b"><soap:address location="test://"/></port></service>
</definitions>
XML;
}

function occurrence_schema(string $attribute, string $value = "2147483648"): string {
    return <<<XML
<xsd:complexType name="T">
  <xsd:sequence>
    <xsd:element name="x" type="xsd:string" $attribute="$value"/>
  </xsd:sequence>
</xsd:complexType>
XML;
}

function restriction_schema(string $facet, string $value = "2147483648"): string {
    return <<<XML
<xsd:simpleType name="T">
  <xsd:restriction base="xsd:int">
    <xsd:$facet value="$value"/>
  </xsd:restriction>
</xsd:simpleType>
XML;
}

$cases = [
    "minOccurs" => occurrence_schema("minOccurs"),
    "maxOccurs" => occurrence_schema("maxOccurs"),
    "negative minOccurs" => occurrence_schema("minOccurs", "-1"),
    "negative maxOccurs" => occurrence_schema("maxOccurs", "-1"),
    "minExclusive" => restriction_schema("minExclusive"),
    "minInclusive" => restriction_schema("minInclusive"),
    "maxExclusive" => restriction_schema("maxExclusive"),
    "maxInclusive" => restriction_schema("maxInclusive"),
    "totalDigits" => restriction_schema("totalDigits"),
    "fractionDigits" => restriction_schema("fractionDigits"),
    "length" => restriction_schema("length"),
    "minLength" => restriction_schema("minLength"),
    "maxLength" => restriction_schema("maxLength"),
];

$numeric_string_cases = [
    "leading whitespace numeric-string" => " 2147483648",
    "leading plus numeric-string" => "+2147483648",
    "leading zero numeric-string" => "00000000002147483648",
    "leading numeric-string with trailing data" => "2147483648abc",
    "negative out-of-range numeric-string" => "-2147483649",
    "decimal numeric-string" => "2147483648.0",
    "exponent numeric-string" => "2147483648e0",
];

foreach ($numeric_string_cases as $name => $value) {
    $cases[$name] = occurrence_schema("maxOccurs", $value);
}

$cases["fractional numeric-string within int range"] = occurrence_schema("maxOccurs", "3.141");

foreach ($cases as $name => $schema) {
    $file = tempnam(sys_get_temp_dir(), "wsdl");
    file_put_contents($file, wsdl_with_schema($schema));

    try {
        new SoapClient($file, ["cache_wsdl" => WSDL_CACHE_NONE]);
        echo "$name: parsed\n";
    } catch (SoapFault $e) {
        echo "$name: {$e->getMessage()}\n";
    } finally {
        unlink($file);
    }
}
?>
--EXPECT--
minOccurs: SOAP-ERROR: Parsing Schema: minOccurs value is out of range
maxOccurs: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
negative minOccurs: SOAP-ERROR: Parsing Schema: minOccurs value is out of range
negative maxOccurs: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
minExclusive: SOAP-ERROR: Parsing Schema: minExclusive value is out of range
minInclusive: SOAP-ERROR: Parsing Schema: minInclusive value is out of range
maxExclusive: SOAP-ERROR: Parsing Schema: maxExclusive value is out of range
maxInclusive: SOAP-ERROR: Parsing Schema: maxInclusive value is out of range
totalDigits: SOAP-ERROR: Parsing Schema: totalDigits value is out of range
fractionDigits: SOAP-ERROR: Parsing Schema: fractionDigits value is out of range
length: SOAP-ERROR: Parsing Schema: length value is out of range
minLength: SOAP-ERROR: Parsing Schema: minLength value is out of range
maxLength: SOAP-ERROR: Parsing Schema: maxLength value is out of range
leading whitespace numeric-string: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
leading plus numeric-string: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
leading zero numeric-string: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
leading numeric-string with trailing data: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
negative out-of-range numeric-string: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
decimal numeric-string: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
exponent numeric-string: SOAP-ERROR: Parsing Schema: maxOccurs value is out of range
fractional numeric-string within int range: parsed
