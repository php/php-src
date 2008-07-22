--TEST--
Bug #38055 (Wrong interpretation of boolean parameters)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function Test($param) {
	global $g1, $g2;
	$g1 = $param->boolA;
	$g2	= $param->boolB;
	return 1;
}

class TestSoapClient extends SoapClient {
  function __construct($wsdl) {
    parent::__construct($wsdl);
    $this->server = new SoapServer($wsdl);
    $this->server->addFunction('Test');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }
}

$client = new TestSoapClient(dirname(__FILE__).'/bug38055.wsdl');
$boolA = 1;
$boolB = '1';
$res = $client->Test(array('boolA'=>$boolA, 'boolB'=>$boolB));
var_dump($g1);
var_dump($g2);
?>
--EXPECT--
bool(true)
bool(true)
