--TEST--
Bug #38004 (Parameters in SoapServer are decoded twice)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function Test($param) {
    global $g;
    $g = $param->strA."\n".$param->strB."\n";
    return $g;
}

class TestSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl) {
    parent::__construct($wsdl);
    $this->server = new SoapServer($wsdl);
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

$client = new TestSoapClient(__DIR__.'/bug38004.wsdl');
$strA = 'test &amp; test';
$strB = 'test & test';
$res = $client->Test(array('strA'=>$strA, 'strB'=>$strB));
print_r($res);
print_r($g);
?>
--EXPECT--
test &amp; test
test & test
test &amp; test
test & test
