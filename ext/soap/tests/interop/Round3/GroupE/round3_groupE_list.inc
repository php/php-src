<?php
class SOAP_Interop_GroupE {

    function echoLinkedList($inputList)
    {
      global $d;
      $d = $inputList;
      return $inputList;
    }

}

$server = new SoapServer(dirname(__FILE__)."/round3_groupE_list.wsdl");
$server->setClass("SOAP_Interop_GroupE");
$server->handle($HTTP_RAW_POST_DATA);
var_dump($d);
?>