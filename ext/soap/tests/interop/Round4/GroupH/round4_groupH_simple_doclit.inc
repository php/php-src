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

  function echoMultipleFaults1($input) {
  	if ($input->whichFault == 2) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, $input->param1, "SimpleFault2");
  	} else if ($input->whichFault == 3) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, $input->param2, "SimpleFault3");
  	} else {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, null, "SimpleFault1");
	  }
  }

  function echoMultipleFaults2($input) {
  	if ($input->whichFault == 2) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $input->param1, "SimpleFault1");
  	} else if ($input->whichFault == 3) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $input->param3, "SimpleFault3");
  	} else {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $input->param2, "SimpleFault2");
	  }
  }

  function echoMultipleFaults3($input) {
  	if ($input->whichFault == 2) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults3'.", null, $input->param2, "SimpleFault2");
	  } else {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults3'.", null, $input->param1, "SimpleFault1");
	  }
  }

  function echoMultipleFaults4($input) {
  	if ($input->whichFault == 2) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults4'.", null, $input->param2, "SimpleFault2");
	  } else {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults4'.", null, $input->param1, "SimpleFault1");
	  }
  }
}

$server = new SoapServer(dirname(__FILE__)."/round4_groupH_simple_doclit.wsdl");
$server->setClass("SOAP_Interop_GroupH");
$server->handle($HTTP_RAW_POST_DATA);
?>
