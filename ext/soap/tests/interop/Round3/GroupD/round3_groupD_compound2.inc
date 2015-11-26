<?php
class SOAP_Interop_GroupD {
    function echoEmployee($employee)
    {
        return $employee;
    }
}

$server = new SoapServer(dirname(__FILE__)."/round3_groupD_compound2.wsdl");
$server->setClass("SOAP_Interop_GroupD");
$server->handle($HTTP_RAW_POST_DATA);
?>
