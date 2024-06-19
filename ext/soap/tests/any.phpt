--TEST--
SOAP handling of <any>
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPComplexType {
    function __construct(public $varString, public $varInt, public $varFloat) {}
}
$struct = new SOAPComplexType('arg',34,325.325);

function echoAnyElement($x) {
    global $g;

    $g = $x;
    $struct = $x->inputAny->any["SOAPComplexType"];
    if ($struct instanceof SOAPComplexType) {
        return array("return" => array("any" => array("SOAPComplexType"=>new SoapVar($struct, SOAP_ENC_OBJECT, "SOAPComplexType", "http://soapinterop.org/xsd", "SOAPComplexType", "http://soapinterop.org/"))));
    } else {
        return "?";
    }
}

class TestSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('echoAnyElement');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): string {
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
    return $response;
  }
}

$client = new TestSoapClient(__DIR__."/interop/Round4/GroupI/round4_groupI_xsd.wsdl",
                             array("trace"=>1,"exceptions"=>0,
                             'classmap' => array('SOAPComplexType'=>'SOAPComplexType')));
$ret = $client->echoAnyElement(
  array(
    "inputAny"=>array(
       "any"=>new SoapVar($struct, SOAP_ENC_OBJECT, "SOAPComplexType", "http://soapinterop.org/xsd", "SOAPComplexType", "http://soapinterop.org/")
     )));
var_dump($g);
var_dump($ret);
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["inputAny"]=>
  object(stdClass)#%d (%d) {
    ["any"]=>
    array(1) {
      ["SOAPComplexType"]=>
      object(SOAPComplexType)#%d (%d) {
        ["varString"]=>
        string(3) "arg"
        ["varInt"]=>
        int(34)
        ["varFloat"]=>
        float(325.325)
      }
    }
  }
}
object(stdClass)#%d (%d) {
  ["return"]=>
  object(stdClass)#%d (%d) {
    ["any"]=>
    array(1) {
      ["SOAPComplexType"]=>
      object(SOAPComplexType)#%d (%d) {
        ["varString"]=>
        string(3) "arg"
        ["varInt"]=>
        int(34)
        ["varFloat"]=>
        float(325.325)
      }
    }
  }
}
