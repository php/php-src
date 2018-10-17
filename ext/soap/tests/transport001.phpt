--TEST--
SOAP Transport 1: Local transport using SoapClient::__doRequest
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function Add($x,$y) {
  return $x+$y;
}

class LocalSoapClient extends SoapClient {

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('Add');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }

}

$x = new LocalSoapClient(NULL,array('location'=>'test://',
                                   'uri'=>'http://testuri.org'));
var_dump($x->Add(3,4));
echo "ok\n";
?>
--EXPECT--
int(7)
ok
