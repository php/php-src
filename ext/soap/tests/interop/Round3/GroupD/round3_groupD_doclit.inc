<?php
class SOAP_Interop_GroupD {

    function echoString($inputString)
    {
      return $inputString;
    }

    function echoStringArray($inputStringArray)
    {
      return $inputStringArray;
    }

    function echoStruct($inputStruct)
    {
      return $inputStruct;
    }

    function echoVoid()
    {
      return NULL;
    }
}

$server = new SoapServer(dirname(__FILE__)."/round3_groupD_doclit.wsdl");
$server->setClass("SOAP_Interop_GroupD");
$server->handle($HTTP_RAW_POST_DATA);
?>
