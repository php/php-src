--TEST--
SOAP XML Schema: self-referential group fix-up recursion is rejected
--EXTENSIONS--
soap
--FILE--
<?php
$wsdl = '<?xml version="1.0"?>
<definitions xmlns="http://schemas.xmlsoap.org/wsdl/"
             xmlns:tns="urn:test" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
 <types>
  <xsd:schema targetNamespace="urn:test" xmlns="http://www.w3.org/2001/XMLSchema">
   <group name="A">
    <sequence>
     <element name="a" type="string"/>
     <group ref="tns:A"/>
    </sequence>
   </group>
   <element name="root">
    <complexType><group ref="tns:A"/></complexType>
   </element>
  </xsd:schema>
 </types>
 <message name="m"><part name="p" element="tns:root"/></message>
 <portType name="pt"><operation name="op"><input message="tns:m"/></operation></portType>
 <binding name="b" type="tns:pt"><soap:binding style="document" transport="http://schemas.xmlsoap.org/soap/http" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"/>
  <operation name="op"><soap:operation soapAction="" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"/><input><soap:body use="literal" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"/></input></operation>
 </binding>
 <service name="s"><port name="p1" binding="tns:b"><soap:address location="http://localhost/x" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"/></port></service>
</definitions>';
$file = __DIR__ . '/schema-selfref-group.wsdl';
file_put_contents($file, $wsdl);
try {
	$c = new SoapClient($file, ['exceptions' => true, 'cache_wsdl' => WSDL_CACHE_NONE]);
	echo "parsed ok\n";
} catch (Throwable $e) {
	echo $e::class, ": ", substr($e->getMessage(), 0, 120), "\n";
}
echo "done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/schema-selfref-group.wsdl');
?>
--EXPECTF--
SoapFault: SOAP-ERROR: Parsing Schema: recursive group 'ref' attribute '%s'
done
