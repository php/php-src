--TEST--
Bug #32941 (Sending structured exception kills a php)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class TestSoapClient extends SoapClient {
  function __doRequest($request, $location, $action, $version, $one_way = 0) {
  	return <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope
xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
	<soapenv:Body>
		<soapenv:Fault>
			<faultcode>soapenv:Server.userException</faultcode>
			<faultstring>service.EchoServiceException</faultstring>
			<detail>
				<service.EchoServiceException xsi:type="ns1:EchoServiceException" xmlns:ns1="urn:service.EchoService">
					<intParameter xsi:type="xsd:int">105</intParameter>
					<parameter xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">string param</parameter>
				</service.EchoServiceException>
				<ns2:hostname xmlns:ns2="http://xml.apache.org/axis/">steckovic</ns2:hostname>
			</detail>
		</soapenv:Fault>
	</soapenv:Body>
</soapenv:Envelope>
EOF;
	}
}

ini_set("soap.wsdl_cache_enabled", 1);
$client = new TestSoapClient(dirname(__FILE__).'/bug32941.wsdl', array("trace" => 1, 'exceptions' => 0));
$ahoj = $client->echoString('exception');
$client = new TestSoapClient(dirname(__FILE__).'/bug32941.wsdl', array("trace" => 1, 'exceptions' => 0));
$ahoj = $client->echoString('exception');
var_dump($ahoj->detail);
echo "ok\n";
?>
--EXPECT--
object(stdClass)#1 (2) {
  [u"service.EchoServiceException"]=>
  object(SoapVar)#8 (4) {
    [u"enc_type"]=>
    int(0)
    [u"enc_value"]=>
    object(stdClass)#7 (2) {
      [u"intParameter"]=>
      int(105)
      [u"parameter"]=>
      unicode(12) "string param"
    }
    [u"enc_stype"]=>
    unicode(20) "EchoServiceException"
    [u"enc_ns"]=>
    unicode(23) "urn:service.EchoService"
  }
  [u"hostname"]=>
  unicode(9) "steckovic"
}
ok
