<?php
class SOAP_Interop_GroupH {

    function echoVersionMismatchFault()
    {
    }

    function echoMustUnderstandFault()
    {
    }

    function HeaderRequest($string)
    {
    }


}

$server = new SoapServer(dirname(__FILE__)."/round4_groupH_soapfault.wsdl");
$server->setClass("SOAP_Interop_GroupH");
$server->handle($HTTP_RAW_POST_DATA);
?>
