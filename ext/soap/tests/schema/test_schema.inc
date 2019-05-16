<?php
$val = null;

function test($input) {
	global $val;
	$val = $input;
}

function test_schema($schema,$type,$param,$style="rpc",$use="encoded", $attributeFormDefault='',$features=0) {
  global $HTTP_RAW_POST_DATA, $val;
$wsdl  = <<<EOF
<definitions name="InteropTest"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
    xmlns:tns="http://test-uri/"
    xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
    xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"
    xmlns="http://schemas.xmlsoap.org/wsdl/"
    targetNamespace="http://test-uri/"
    >
  <types>
  <schema xmlns="http://www.w3.org/2001/XMLSchema" targetNamespace="http://test-uri/" $attributeFormDefault>
   <xsd:import namespace="http://schemas.xmlsoap.org/soap/encoding/" />
   <xsd:import namespace="http://schemas.xmlsoap.org/wsdl/" />
  	$schema
  </schema>
  </types>
  <message name="testMessage">
  	<part name="testParam" $type/>
  </message>
	<portType name="testPortType">
		<operation name="test">
			<input message="testMessage"/>
		</operation>
	</portType>
	<binding name="testBinding" type="testPortType">
		<soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
		<operation name="test">
			<soap:operation soapAction="#test" style="$style"/>
			<input>
				<soap:body use="$use" namespace="http://test-uri/" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
			</input>
		</operation>
	</binding>
	<service name="testService">
   <port name="testPort" binding="tns:testBinding">
     <soap:address location="test://" />
   </port>
 </service>
</definitions>
EOF;

  $fname = tempnam ("./", "wsdl");
  $f = fopen($fname,"w");
  fwrite($f,$wsdl);
  fclose($f);
  ini_set("soap.wsdl_cache_enabled",0);
  $x = new SoapClient($fname, array("trace"=>1,"exceptions"=>0,"features"=>$features));
  $y = new SoapServer($fname, array("features"=>$features));
	$y->addfunction("test");
  unlink($fname);

  $x->test($param);
  $xml = xml_parser_create();
  $req = $x->__getlastrequest();
  if ($style == "rpc") {
    $HTTP_RAW_POST_DATA = $req;
    ob_start();
    $y->handle($HTTP_RAW_POST_DATA);
    ob_end_clean();
    echo $req;
    var_dump($val);
  } else {
    echo $req;
  }
}
?>
