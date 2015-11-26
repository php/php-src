<?php
class SOAP_Interop_GroupD {

    function echoStruct($inputStruct)
    {
      return $inputStruct;
    }

    function echoStructArray($inputStructArray)
    {
      return $inputStructArray;
    }

}

$server = new SoapServer(dirname(__FILE__)."/round3_groupD_import3.wsdl");
$server->setClass("SOAP_Interop_GroupD");
$server->handle($HTTP_RAW_POST_DATA);
?>