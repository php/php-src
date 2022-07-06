<?php
class SOAP_Interop_GroupH {

  function echoEmptyFault() {
    return new SoapFault("Server", "Fault in response to 'echoEmptyFault'.", null, null, "SimpleFault");
  }

  function echoStringFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoStringFault'.", null, $input, "SimpleFault");
  }

  function echoIntArrayFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoIntArrayFault'.", null, $input, "SimpleFault");
  }

  function echoMultipleFaults1($param, $string, $floats) {
    if ($param == 2) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, $string, "SimpleFault2");
    } else if ($param == 3) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, $floats, "SimpleFault3");
    } else {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, null, "SimpleFault1");
      }
  }

  function echoMultipleFaults2($param, $string, $float, $strings) {
    if ($param == 2) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $string, "SimpleFault1");
    } else if ($param == 3) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $strings, "SimpleFault3");
    } else {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $float, "SimpleFault2");
      }
  }

  function echoMultipleFaults3($param, $param1, $param2) {
    if ($param == 2) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults3'.", null, $param2, "SimpleFault2");
      } else {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults3'.", null, $param1, "SimpleFault1");
      }
  }

  function echoMultipleFaults4($param, $int, $enum) {
    if ($param == 2) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults4'.", null, $enum, "SimpleFault2");
      } else {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults4'.", null, $int, "SimpleFault1");
      }
  }
}

$server = new SoapServer(dirname(__FILE__)."/round4_groupH_simple_rpcenc.wsdl");
$server->setClass("SOAP_Interop_GroupH");
$server->handle($HTTP_RAW_POST_DATA);
?>
