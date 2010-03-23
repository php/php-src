<?php
class SOAP_Interop_GroupH {

  function echoSOAPStructFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoSOAPStructFault'.", null, $input, "ComplexFault");
  }

  function echoBaseStructFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoBaseStructFault'.", null, $input, "ComplexFault");
  }

  function echoExtendedStructFault($input) {
    return new SoapFault("Server", "Fault in response to 'echoExtendedStructFault'.", null, $input, "ComplexFault");
  }

  function echoMultipleFaults1($which, $s1, $s2) {
  	if ($which == 2) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, $s2, "ComplexFault2");
	  } else {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults1'.", null, array("soapStruct"=>$s1), "ComplexFault1");
	  }
  }

  function echoMultipleFaults2($which, $s1, $s2, $s3) {
  	if ($which == 2) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $s2, "ComplexFault2");
  	} else if ($which == 3) {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $s3, "ComplexFault3");
	  } else {
	    return new SoapFault("Server", "Fault in response to 'echoMultipleFaults2'.", null, $s1, "ComplexFault1");
	  }
  }
}

$server = new SoapServer(dirname(__FILE__)."/round4_groupH_complex_rpcenc.wsdl");
$server->setClass("SOAP_Interop_GroupH");
$server->handle($HTTP_RAW_POST_DATA);
?>
