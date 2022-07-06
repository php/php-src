<?php
class SOAP_Interop_GroupH {

  function echoSOAPStructFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoSOAPStructFault'.", null, array("soapStruct"=>$input), "ComplexFault");
  }

  function echoBaseStructFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoBaseStructFault'.", null, $input, "ComplexFault");
  }

  function echoExtendedStructFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoExtendedStructFault'.", null, $input, "ComplexFault");
  }

  function echoMultipleFaults1($input) {
    if ($input->whichFault == 2) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, $input->param2, "ComplexFault2");
      } else {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, array("soapStruct"=>$input->param1), "ComplexFault1");
      }
  }

  function echoMultipleFaults2($input) {
    if ($input->whichFault == 2) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $input->param2, "ComplexFault2");
    } else if ($input->whichFault == 3) {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $input->param3, "ComplexFault3");
      } else {
        return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $input->param1, "ComplexFault1");
      }
  }
}

$server = new SoapServer(dirname(__FILE__)."/round4_groupH_complex_doclit.wsdl");
$server->setClass("SOAP_Interop_GroupH");
$server->handle($HTTP_RAW_POST_DATA);
?>
