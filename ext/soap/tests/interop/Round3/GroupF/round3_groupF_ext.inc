<?php
class SOAP_Interop_GroupF {

    function echoString($inputString)
    {
      return $inputString;
    }

}

$server = new SoapServer(dirname(__FILE__)."/round3_groupF_ext.wsdl");
$server->setClass("SOAP_Interop_GroupF");
$server->handle($HTTP_RAW_POST_DATA);
?>
