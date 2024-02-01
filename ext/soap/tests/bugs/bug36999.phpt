--TEST--
Bug #36999 (xsd:long values clamped to LONG_MAX instead of using double)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

function echoLong($num) {
  return $num;
}

class LocalSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl) {
    parent::__construct($wsdl);
    $this->server = new SoapServer($wsdl);
    $this->server->addFunction('echoLong');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }

}

$soap = new LocalSoapClient(__DIR__."/bug36999.wsdl");

function test($num) {
  global $soap;
  try {
      printf("%s %0.0f\n", gettype($num), $num);
      $ret = $soap->echoLong($num);
      printf("%s %0.0f\n", gettype($ret), $ret);
    } catch (SoapFault $ex) {
      var_dump($ex);
    }
}
test(3706790240);
?>
--EXPECTF--
%s 3706790240
%s 3706790240
