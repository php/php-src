--TEST--
Bug #29839 (incorrect convert (xml:lang to lang))
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

function EchoString($s) {
  return $s;
}

class LocalSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('EchoString');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }

}

$client = new LocalSoapClient(__DIR__."/bug34453.wsdl", array("trace"=>1));
$client->EchoString(array("value"=>"hello","lang"=>"en"));
echo $client->__getLastRequest();
echo $client->__getLastResponse();
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri"><SOAP-ENV:Body><string xml:lang="en"><ns1:value>hello</ns1:value></string></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri"><SOAP-ENV:Body><string xml:lang="en"><ns1:value>hello</ns1:value></string></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
