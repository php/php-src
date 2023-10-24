--TEST--
SOAP Classmap 7: encoding of inherited objects with namespace and cache wsdl
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=1
--FILE--
<?php
class A {
  public $x;
  function __construct($a){
    $this->x = $a;
  }
}
class Attest {
  public $x;
  function __construct($a){
    $this->x = $a;
  }
}
class B extends A {
  public $y;
  function __construct($a){
    parent::__construct($a);
    $this->y = $a + 1;
  }
}

function f($input){
  return new B(5);
}

class LocalSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction("f");
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }
}

$client = new LocalSoapClient(__DIR__."/classmap007.wsdl",
    array('classmap'=>array('A'=>'A','{urn:abt}At'=>'Attest','B'=>'B')));
print_r($client->f(new Attest('test')));
print_r($client->f(new Attest('test')));
?>
--EXPECT--
B Object
(
    [x] => 5
    [y] => 6
)
B Object
(
    [x] => 5
    [y] => 6
)
