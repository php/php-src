--TEST--
SOAP Classmap 4: encoding of objects with __get()
--EXTENSIONS--
soap
--FILE--
<?php
ini_set("soap.wsdl_cache_enabled",0);

class A {
  public $a;
  function __construct($a){
    $this->x = $a;
  }
  function __get($name) {
    return @$this->a[$name];
  }
  function __set($name, $val) {
    $this->a[$name] = $val;
  }
  function __unset($name) {
    unset($this->a[$name]);
  }
}

class B extends A {
  function __construct($a){
    parent::__construct($a);
    $this->y = $a + 1;
  }
}

function f(){
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

$client = new LocalSoapClient(__DIR__."/classmap003.wsdl",
    array('classmap'=>array('A'=>'A','B'=>'B')));
print_r($client->f());
?>
--EXPECT--
B Object
(
    [a] => Array
        (
            [x] => 5
            [y] => 6
        )

)
