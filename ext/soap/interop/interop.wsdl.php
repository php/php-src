<?php
header("Content-Type: text/xml");
echo '<?xml version="1.0"?>';
echo "\n";
?>
<definitions name="InteropTest"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
    xmlns:tns="http://soapinterop.org/"
    xmlns:s="http://soapinterop.org/xsd"
    xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
    xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"
    xmlns="http://schemas.xmlsoap.org/wsdl/"
    targetNamespace="http://soapinterop.org/">

  <types>
  <schema xmlns="http://www.w3.org/2001/XMLSchema" targetNamespace="http://soapinterop.org/xsd">
   <xsd:import namespace="http://schemas.xmlsoap.org/soap/encoding/" />
   <xsd:import namespace="http://schemas.xmlsoap.org/wsdl/" />
   <xsd:complexType name="ArrayOfstring">
    <xsd:complexContent>
     <xsd:restriction base="SOAP-ENC:Array">
      <xsd:attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="string[]"/>
     </xsd:restriction>
    </xsd:complexContent>
   </xsd:complexType>
   <xsd:complexType name="ArrayOfint">
    <xsd:complexContent>
     <xsd:restriction base="SOAP-ENC:Array">
      <xsd:attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="int[]"/>
     </xsd:restriction>
    </xsd:complexContent>
   </xsd:complexType>
   <xsd:complexType name="ArrayOffloat">
    <xsd:complexContent>
     <xsd:restriction base="SOAP-ENC:Array">
      <xsd:attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="float[]"/>
     </xsd:restriction>
    </xsd:complexContent>
   </xsd:complexType>
   <xsd:complexType name="SOAPStruct">
    <xsd:all>
     <xsd:element name="varString" type="string"/>
     <xsd:element name="varInt" type="int"/>
     <xsd:element name="varFloat" type="float"/>
    </xsd:all>
   </xsd:complexType>
   <xsd:complexType name="ArrayOfSOAPStruct">
    <xsd:complexContent>
     <xsd:restriction base="SOAP-ENC:Array">
      <xsd:attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="s:SOAPStruct[]"/>
     </xsd:restriction>
    </xsd:complexContent>
   </xsd:complexType>
  </schema>
  </types>

  <message name="echoStringRequest">
    <part name="inputString" type="xsd:string" />
  </message>
  <message name="echoStringResponse">
    <part name="outputString" type="xsd:string" />
  </message>
  <message name="echoStringArrayRequest">
    <part name="inputStringArray" type="s:ArrayOfstring" />
  </message>
  <message name="echoStringArrayResponse">
    <part name="outputStringArray" type="s:ArrayOfstring" />
  </message>
  <message name="echoIntegerRequest">
    <part name="inputInteger" type="xsd:int" />
  </message>
  <message name="echoIntegerResponse">
    <part name="outputInteger" type="xsd:int" />
  </message>
  <message name="echoIntegerArrayRequest">
    <part name="inputIntegerArray" type="s:ArrayOfint" />
  </message>
  <message name="echoIntegerArrayResponse">
    <part name="outputIntegerArray" type="s:ArrayOfint" />
  </message>
  <message name="echoFloatRequest">
    <part name="inputFloat" type="xsd:float" />
  </message>
  <message name="echoFloatResponse">
    <part name="outputFloat" type="xsd:float" />
  </message>
  <message name="echoFloatArrayRequest">
    <part name="inputFloatArray" type="s:ArrayOffloat" />
  </message>
  <message name="echoFloatArrayResponse">
    <part name="outputFloatArray" type="s:ArrayOffloat" />
  </message>
  <message name="echoStructRequest">
    <part name="inputStruct" type="s:SOAPStruct" />
  </message>
  <message name="echoStructResponse">
    <part name="outputStruct" type="s:SOAPStruct" />
  </message>
  <message name="echoStructArrayRequest">
    <part name="inputStructArray" type="s:ArrayOfSOAPStruct" />
  </message>
  <message name="echoStructArrayResponse">
    <part name="outputStructArray" type="s:ArrayOfSOAPStruct" />
  </message>
  <message name="echoVoidRequest">
  </message>
  <message name="echoVoidResponse">
  </message>
  <message name="echoBase64Request">
    <part name="inputBase64" type="xsd:base64Binary" />
  </message>
  <message name="echoBase64Response">
    <part name="outputBase64" type="xsd:base64Binary" />
  </message>
  <message name="echoDateRequest">
    <part name="inputDate" type="xsd:dateTime" />
  </message>
  <message name="echoDateResponse">
    <part name="outputDate" type="xsd:dateTime" />
  </message>
  <message name="echoHexBinaryRequest">
    <part name="inputHexBinary" type="xsd:hexBinary" />
  </message>
  <message name="echoHexBinaryResponse">
    <part name="outputHexBinary" type="xsd:hexBinary" />
  </message>
  <message name="echoDecimalRequest">
    <part name="inputDecimal" type="xsd:decimal" />
  </message>
  <message name="echoDecimalResponse">
    <part name="outputDecimal" type="xsd:decimal" />
  </message>
  <message name="echoBooleanRequest">
    <part name="inputBoolean" type="xsd:boolean" />
  </message>
  <message name="echoBooleanResponse">
    <part name="outputBoolean" type="xsd:boolean" />
  </message>

  <portType name="InteropTestPortType">
    <operation name="echoString">
      <input message="tns:echoStringRequest"/>
      <output message="tns:echoStringResponse"/>
    </operation>
    <operation name="echoStringArray">
      <input message="tns:echoStringArrayRequest"/>
      <output message="tns:echoStringArrayResponse"/>
    </operation>
    <operation name="echoInteger">
      <input message="tns:echoIntegerRequest"/>
      <output message="tns:echoIntegerResponse"/>
    </operation>
    <operation name="echoIntegerArray">
      <input message="tns:echoIntegerArrayRequest"/>
      <output message="tns:echoIntegerArrayResponse"/>
    </operation>
    <operation name="echoFloat">
      <input message="tns:echoFloatRequest"/>
      <output message="tns:echoFloatResponse"/>
    </operation>
    <operation name="echoFloatArray">
      <input message="tns:echoFloatArrayRequest"/>
      <output message="tns:echoFloatArrayResponse"/>
    </operation>
    <operation name="echoStruct">
      <input message="tns:echoStructRequest"/>
      <output message="tns:echoStructResponse"/>
    </operation>
    <operation name="echoStructArray">
      <input message="tns:echoStructArrayRequest"/>
      <output message="tns:echoStructArrayResponse"/>
    </operation>
    <operation name="echoVoid">
      <input message="tns:echoVoidRequest"/>
      <output message="tns:echoVoidResponse"/>
    </operation>
    <operation name="echoBase64">
      <input message="tns:echoBase64Request"/>
      <output message="tns:echoBase64Response"/>
    </operation>
    <operation name="echoDate">
      <input message="tns:echoDateRequest"/>
      <output message="tns:echoDateResponse"/>
    </operation>
    <operation name="echoHexBinary">
      <input message="tns:echoHexBinaryRequest"/>
      <output message="tns:echoHexBinaryResponse"/>
    </operation>
    <operation name="echoDecimal">
      <input message="tns:echoDecimalRequest"/>
      <output message="tns:echoDecimalResponse"/>
    </operation>
    <operation name="echoBoolean">
      <input message="tns:echoBooleanRequest"/>
      <output message="tns:echoBooleanResponse"/>
    </operation>
  </portType>

  <binding name="InteropTestBinding" type="tns:InteropTestPortType">
    <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
    <operation name="echoString">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoStringArray">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoInteger">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoIntegerArray">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoFloat">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoFloatArray">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoStruct">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoStructArray">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoVoid">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoBase64">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoDate">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoHexBinary">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoDecimal">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
    <operation name="echoBoolean">
      <soap:operation soapAction="http://" style="rpc"/>
      <input>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </input>
      <output>
        <soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
      </output>
    </operation>
  </binding>

  <service name="InteropTest">
    <port name="InteropTestPort" binding="tns:InteropTestBinding">
			<soap:address location="<?php echo ((isset($_SERVER['HTTPS'])?"https://":"http://").$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF']));?>/server_round2_base.php"/>
    </port>
  </service>

</definitions>
