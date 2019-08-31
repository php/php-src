--TEST--
SOAP handling of <any>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPComplexType {
    function __construct($s, $i, $f) {
        $this->varString = $s;
        $this->varInt = $i;
        $this->varFloat = $f;
    }
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
  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('echoAnyElement');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
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
--EXPECT--
object(stdClass)#5 (1) {
  ["inputAny"]=>
  object(stdClass)#6 (1) {
    ["any"]=>
    array(1) {
      ["SOAPComplexType"]=>
      object(SOAPComplexType)#7 (3) {
        ["varInt"]=>
        int(34)
        ["varString"]=>
        string(3) "arg"
        ["varFloat"]=>
        float(325.325)
      }
    }
  }
}
object(stdClass)#8 (1) {
  ["return"]=>
  object(stdClass)#9 (1) {
    ["any"]=>
    array(1) {
      ["SOAPComplexType"]=>
      object(SOAPComplexType)#10 (3) {
        ["varInt"]=>
        int(34)
        ["varString"]=>
        string(3) "arg"
        ["varFloat"]=>
        float(325.325)
      }
    }
  }
}
