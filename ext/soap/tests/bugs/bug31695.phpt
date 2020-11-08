--TEST--
Bug #31695 (Cannot redefine endpoint when using WSDL)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
ini_set("soap.wsdl_cache_enabled", 0);

function Test($x) {
    return $x;
}

class LocalSoapClient extends SoapClient {
  function __construct($wsdl, $options=array()) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
        $this->server->addFunction("Test");
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    echo "$location\n";
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }
}

$client = new LocalSoapClient(__DIR__."/bug31695.wsdl");
$client->Test("str");
$client = new LocalSoapClient(__DIR__."/bug31695.wsdl", array("location"=>"test://1"));
$client->Test("str");
$client->__soapCall("Test",
                    array("arg1"),
                     array("location"=>"test://2"));
$old = $client->__setLocation("test://3");
echo "$old\n";
$client->Test("str");
$client->Test("str");
$client->__setLocation($old);
$client->Test("str");
$old = $client->__setLocation();
$client->Test("str");
$client->__setLocation($old);
$client->Test("str");
$client->__setLocation(null);
$client->Test("str");
var_dump($client->__setLocation());
?>
--EXPECT--
test://0
test://1
test://2
test://1
test://3
test://3
test://1
test://0
test://1
test://0
NULL
