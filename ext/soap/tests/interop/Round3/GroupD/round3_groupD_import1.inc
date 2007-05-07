<?php
class SOAP_Interop_GroupD {

    function echoString($inputString)
    {
      return $inputString;
    }

}

$server = new SoapServer(dirname(__FILE__)."/round3_groupD_import1.wsdl");
$server->setClass("SOAP_Interop_GroupD");
$server->handle($HTTP_RAW_POST_DATA);
?>