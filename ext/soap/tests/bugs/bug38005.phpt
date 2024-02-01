--TEST--
Bug #38005 (SoapFault faultstring doesn't follow encoding rules)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function Test($param=NULL) {
    return new SoapFault('Test', 'This is our fault: Ä');
}

class TestSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $opt) {
    parent::__construct($wsdl, $opt);
    $this->server = new SoapServer($wsdl, $opt);
    $this->server->addFunction('Test');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }
}

$client = new TestSoapClient(NULL, array(
    'encoding' => 'ISO-8859-1',
    'uri' => "test://",
    'location' => "test://",
    'soap_version'=>SOAP_1_2,
    'trace'=>1,
    'exceptions'=>0));
$res = $client->Test();
echo($res->faultstring."\n");
echo($client->__getLastResponse());
?>
--EXPECT--
This is our fault: Ä
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>Test</env:Value></env:Code><env:Reason><env:Text>This is our fault: Ã„</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
