--TEST--
Bug #50762 (in WSDL mode Soap Header handler function only being called if defined in WSDL)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class testSoap {
    private $auth;
    public function authToken($token){
        $this->auth=true;
    }
    public function testHeader($param){
        return 'header handler ' . ($this->auth ? 'called' : 'not called');
    }
}

class LocalSoapClient extends SoapClient {

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->setObject(new testSoap());
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }

}

$cl = new LocalSoapClient(__DIR__.'/bug50762.wsdl', array('cache_wsdl'=>WSDL_CACHE_NONE, 'trace'=>true));

class authToken{
    public function __construct($token){
        $this->authToken=$token;
    }
}

$cl->__setSoapHeaders(array(new SoapHeader('http://sova.pronto.ru/', 'authToken', new authToken('tokendata'))));
echo $cl->testHeader('param') . PHP_EOL;
?>
--EXPECT--
header handler called
