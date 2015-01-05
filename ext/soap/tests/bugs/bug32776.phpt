--TEST--
Bug #32776 (SOAP doesn't support one-way operations)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

$d = null;

function test($x) {
	global $d;
	$d = $x;
}

class LocalSoapClient extends SoapClient {

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
  }

}

$x = new LocalSoapClient(dirname(__FILE__)."/bug32776.wsdl",array("trace"=>true,"exceptions"=>false)); 
var_dump($x->test("Hello"));
var_dump($d);
var_dump($x->__getLastRequest());
var_dump($x->__getLastResponse());
echo "ok\n";
?>
--EXPECT--
NULL
string(5) "Hello"
string(459) "<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><SOAP-ENV:test><x xsi:type="xsd:string">Hello</x></SOAP-ENV:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
"
string(0) ""
ok
