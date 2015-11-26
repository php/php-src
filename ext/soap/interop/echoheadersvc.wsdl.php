<?php
header("Content-Type: text/xml");
echo '<?xml version="1.0"?>';
echo "\n";
?>
<definitions name="InteropTest" targetNamespace="http://soapinterop.org/" xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:tns="http://soapinterop.org/" xmlns:s="http://soapinterop.org/xsd" xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/">

	<types>
		<schema xmlns="http://www.w3.org/2001/XMLSchema" targetNamespace="http://soapinterop.org/xsd">

			<import namespace="http://schemas.xmlsoap.org/soap/encoding/" />

			<complexType name="ArrayOfstring">
				<complexContent>
					<restriction base="SOAP-ENC:Array">
						<attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="string[]"/>
					</restriction>
				</complexContent>
			</complexType>
			<complexType name="ArrayOfint">
				<complexContent>
					<restriction base="SOAP-ENC:Array">
						<attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="int[]"/>
					</restriction>
				</complexContent>
			</complexType>
			<complexType name="ArrayOffloat">
				<complexContent>
					<restriction base="SOAP-ENC:Array">
						<attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="float[]"/>
					</restriction>
				</complexContent>
			</complexType>
			<complexType name="ArrayOfSOAPStruct">
				<complexContent>
					<restriction base="SOAP-ENC:Array">
						<attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="s:SOAPStruct[]"/>
					</restriction>
				</complexContent>
			</complexType>
			<complexType name="SOAPStruct">
				<all>
					<element name="varString" type="string" nillable="true"/>
					<element name="varInt" type="int" nillable="true"/>
					<element name="varFloat" type="float" nillable="true"/>
				</all>
			</complexType>
		</schema>
	</types>

	<message name="echoHeaderString_Request">
		<part name="echoMeStringRequest" type="xsd:string"/>
	</message>
	<message name="echoHeaderString_Response">
		<part name="echoMeStringResponse" type="xsd:string"/>
	</message>
	<message name="echoHeaderStruct_Request">
		<part name="echoMeStructRequest" type="s:SOAPStruct"/>
	</message>
	<message name="echoHeaderStruct_Response">
		<part name="echoMeStructResponse" type="s:SOAPStruct"/>
	</message>
	<message name="echoStringRequest">
		<part name="inputString" type="xsd:string"/>
	</message>
	<message name="echoStringResponse">
		<part name="return" type="xsd:string"/>
	</message>
	<message name="echoStringArrayRequest">
		<part name="inputStringArray" type="s:ArrayOfstring"/>
	</message>
	<message name="echoStringArrayResponse">
		<part name="return" type="s:ArrayOfstring"/>
	</message>
	<message name="echoIntegerRequest">
		<part name="inputInteger" type="xsd:int"/>
	</message>
	<message name="echoIntegerResponse">
		<part name="return" type="xsd:int"/>
	</message>
	<message name="echoIntegerArrayRequest">
		<part name="inputIntegerArray" type="s:ArrayOfint"/>
	</message>
	<message name="echoIntegerArrayResponse">
		<part name="return" type="s:ArrayOfint"/>
	</message>
	<message name="echoFloatRequest">
		<part name="inputFloat" type="xsd:float"/>
	</message>
	<message name="echoFloatResponse">
		<part name="return" type="xsd:float"/>
	</message>
	<message name="echoFloatArrayRequest">
		<part name="inputFloatArray" type="s:ArrayOffloat"/>
	</message>
	<message name="echoFloatArrayResponse">
		<part name="return" type="s:ArrayOffloat"/>
	</message>
	<message name="echoStructRequest">
		<part name="inputStruct" type="s:SOAPStruct"/>
	</message>
	<message name="echoStructResponse">
		<part name="return" type="s:SOAPStruct"/>
	</message>
	<message name="echoStructArrayRequest">
		<part name="inputStructArray" type="s:ArrayOfSOAPStruct"/>
	</message>
	<message name="echoStructArrayResponse">
		<part name="return" type="s:ArrayOfSOAPStruct"/>
	</message>
	<message name="echoVoidRequest"/>
	<message name="echoVoidResponse"/>
	<message name="echoBase64Request">
		<part name="inputBase64" type="xsd:base64Binary"/>
	</message>
	<message name="echoBase64Response">
		<part name="return" type="xsd:base64Binary"/>
	</message>
	<message name="echoDateRequest">
		<part name="inputDate" type="xsd:dateTime"/>
	</message>
	<message name="echoDateResponse">
		<part name="return" type="xsd:dateTime"/>
	</message>
	<message name="echoHexBinaryRequest">
		<part name="inputHexBinary" type="xsd:hexBinary"/>
	</message>
	<message name="echoHexBinaryResponse">
		<part name="return" type="xsd:hexBinary"/>
	</message>
	<message name="echoDecimalRequest">
		<part name="inputDecimal" type="xsd:decimal"/>
	</message>
	<message name="echoDecimalResponse">
		<part name="return" type="xsd:decimal"/>
	</message>
	<message name="echoBooleanRequest">
		<part name="inputBoolean" type="xsd:boolean"/>
	</message>
	<message name="echoBooleanResponse">
		<part name="return" type="xsd:boolean"/>
	</message>

	<portType name="InteropTestPortType">
		<operation name="echoString" parameterOrder="inputString">
			<input message="tns:echoStringRequest"/>
			<output message="tns:echoStringResponse"/>
		</operation>
		<operation name="echoStringArray" parameterOrder="inputStringArray">
			<input message="tns:echoStringArrayRequest"/>
			<output message="tns:echoStringArrayResponse"/>
		</operation>
		<operation name="echoInteger" parameterOrder="inputInteger">
			<input message="tns:echoIntegerRequest"/>
			<output message="tns:echoIntegerResponse"/>
		</operation>
		<operation name="echoIntegerArray" parameterOrder="inputIntegerArray">
			<input message="tns:echoIntegerArrayRequest"/>
			<output message="tns:echoIntegerArrayResponse"/>
		</operation>
		<operation name="echoFloat" parameterOrder="inputFloat">
			<input message="tns:echoFloatRequest"/>
			<output message="tns:echoFloatResponse"/>
		</operation>
		<operation name="echoFloatArray" parameterOrder="inputFloatArray">
			<input message="tns:echoFloatArrayRequest"/>
			<output message="tns:echoFloatArrayResponse"/>
		</operation>
		<operation name="echoStruct" parameterOrder="inputStruct">
			<input message="tns:echoStructRequest"/>
			<output message="tns:echoStructResponse"/>
		</operation>
		<operation name="echoStructArray" parameterOrder="inputStructArray">
			<input message="tns:echoStructArrayRequest"/>
			<output message="tns:echoStructArrayResponse"/>
		</operation>
		<operation name="echoVoid">
			<input message="tns:echoVoidRequest"/>
			<output message="tns:echoVoidResponse"/>
		</operation>
		<operation name="echoBase64" parameterOrder="inputBase64">
			<input message="tns:echoBase64Request"/>
			<output message="tns:echoBase64Response"/>
		</operation>
		<operation name="echoDate" parameterOrder="inputDate">
			<input message="tns:echoDateRequest"/>
			<output message="tns:echoDateResponse"/>
		</operation>
		<operation name="echoHexBinary" parameterOrder="inputHexBinary">
			<input message="tns:echoHexBinaryRequest"/>
			<output message="tns:echoHexBinaryResponse"/>
		</operation>
		<operation name="echoDecimal" parameterOrder="inputDecimal">
			<input message="tns:echoDecimalRequest"/>
			<output message="tns:echoDecimalResponse"/>
		</operation>
		<operation name="echoBoolean" parameterOrder="inputBoolean">
			<input message="tns:echoBooleanRequest"/>
			<output message="tns:echoBooleanResponse"/>
		</operation>
	</portType>

	<binding name="InteropEchoHeaderBinding" type="tns:InteropTestPortType">
		<soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>

		<operation name="echoString">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoStringArray">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoInteger">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoIntegerArray">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoFloat">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoFloatArray">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoStruct">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoStructArray">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoVoid">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoBase64">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoDate">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoHexBinary">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoDecimal">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>
		<operation name="echoBoolean">
			<soap:operation soapAction="http://soapinterop.org/"/>
			<input>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Request" part="echoMeStringRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Request" part="echoMeStructRequest" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
			<output>
				<soap:body use="encoded" namespace="http://soapinterop.org/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderString_Response" part="echoMeStringResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
				<soap:header use="encoded" message="tns:echoHeaderStruct_Response" part="echoMeStructResponse" namespace="http://soapinterop.org/echoheader/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</output>
		</operation>

	</binding>

	<service name="interopLabEchoHeader">

  		<port name="interopPortEchoHdr" binding="tns:InteropEchoHeaderBinding">
    			<soap:address location="<?php echo ((isset($_SERVER['HTTPS'])?"https://":"http://").$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF']));?>/server_round2_groupC.php"/>
  		</port>

	</service>
</definitions>
