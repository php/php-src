--TEST--
SoapServer handle with WSDL that has disappeared and disabled readfile function
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") {
    die("skip see https://github.com/php/php-src/issues/17468");
}
?>
--GET--
WSDL
--INI--
disable_functions=readfile
--FILE--
<?php
$wsdlFile = __DIR__ . '/test_handle_non_existent_wsdl_from_get_query_disable_readfile.wsdl';

$wsdl = <<<'WSDL'
<?xml version="1.0" ?>
<definitions
    xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
    xmlns="http://schemas.xmlsoap.org/wsdl/">

  <types>
    <xsd:schema targetNamespace="http://linuxsrv.home/~dmitry/soap/test.wsdl">
      <xsd:import namespace="http://schemas.xmlsoap.org/soap/encoding/" />
      <xsd:import namespace="http://schemas.xmlsoap.org/wsdl/" />
    </xsd:schema>
  </types>

  <message name="AddRequest">
    <part name="x" type="xsd:double" />
    <part name="y" type="xsd:double" />
  </message>
  <message name="AddResponse">
    <part name="result" type="xsd:double" />
  </message>

  <portType name="TestServicePortType">
    <operation name="Add">
      <input message="tns:AddRequest" />
      <output message="tns:AddResponse" />
    </operation>
  </portType>

  <binding name="TestServiceBinding" type="tns:TestServicePortType">
    <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http" />
    <operation name="Add">
      <soap:operation soapAction="Add" style="rpc" />
      <input>
        <soap:body use="encoded" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" />
      </input>
      <output>
        <soap:body use="encoded" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" />
      </output>
    </operation>
  </binding>

  <service name="TestService">
    <port name="TestServicePort" binding="tns:TestServiceBinding">
      <soap:address location="http://linuxsrv.home/~dmitry/soap/soap_server.php" />
    </port>
  </service>

</definitions>
WSDL;

file_put_contents($wsdlFile, $wsdl);

$options = [];
$server = new SoapServer($wsdlFile, $options);

unlink($wsdlFile);
$server->handle();

?>
--CLEAN--
<?php
$wsdlFile = __DIR__ . '/test_handle_non_existent_wsdl_from_get_query_disable_readfile.wsdl';
@unlink($wsdlFile);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Couldn't find WSDL</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
