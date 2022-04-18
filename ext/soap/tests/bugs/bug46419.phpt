--TEST--
Bug #46419 (Elements of associative arrays with NULL value are lost)
--EXTENSIONS--
soap
--FILE--
<?php
function bar() {
  return array('a' => 1, 'b' => NULL, 'c' => 2, 'd'=>'');
}

class LocalSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('bar');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }

}

$x = new LocalSoapClient(NULL,array('location'=>'test://',
                                   'uri'=>'http://testuri.org'));
var_dump($x->bar());
?>
--EXPECT--
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  NULL
  ["c"]=>
  int(2)
  ["d"]=>
  string(0) ""
}
