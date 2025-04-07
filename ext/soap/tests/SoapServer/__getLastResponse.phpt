--TEST--
Request #47317 (SoapServer::__getLastResponse)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function f() {
}

class LocalSoapClient extends SoapClient {
  public $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction("f");
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }
}

$client = new LocalSoapClient(__DIR__."/../classmap003.wsdl", ["trace" => false]);
$client->f();
var_dump($client->__getLastResponse());
var_dump($client->server->__getLastResponse());
var_dump($client->__getLastResponse() === $client->server->__getLastResponse());

echo "---\n";

$client = new LocalSoapClient(__DIR__."/../classmap003.wsdl", ["trace" => true]);
var_dump($client->__getLastResponse());
var_dump($client->server->__getLastResponse());
var_dump($client->__getLastResponse() === $client->server->__getLastResponse());

echo "---\n";

$client->f();
echo $client->__getLastResponse(), "\n";
echo $client->server->__getLastResponse(), "\n";
var_dump($client->__getLastResponse() === $client->server->__getLastResponse());
?>
--EXPECT--
NULL
NULL
bool(true)
---
NULL
NULL
bool(true)
---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:ab" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:fResponse><fReturn xsi:nil="true" xsi:type="ns1:A"/></ns1:fResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>

<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:ab" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:fResponse><fReturn xsi:nil="true" xsi:type="ns1:A"/></ns1:fResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>

bool(true)
