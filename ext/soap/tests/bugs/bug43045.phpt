--TEST--
Bug #43045i (SOAP encoding violation on "INF" for type double/float)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test($x) {
  return $x;
}

class TestSoapClient extends SoapClient {
  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('test');
  }
  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;

    echo $request;
    return '<soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
soap:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
<soap:Body><testResponse xmlns="urn:TestSOAP">
<s-gensym3>
<doubleInfinity xsi:type="xsd:double">INF</doubleInfinity>
</s-gensym3>
</testResponse>
</soap:Body></soap:Envelope>';
  }
}
$client = new TestSoapClient(NULL, array(
			"location" => "test://",
			"uri"      => 'urn:TestSOAP',
			"style"    => SOAP_RPC,
			"use"      => SOAP_ENCODED
			));
var_dump($client->test(0.1));
var_dump($client->test(NAN));
var_dump($response = $client->test(INF));
var_dump($response = $client->test(-INF));
--EXPECT--
float(0.1)
float(NAN)
float(INF)
float(-INF)
