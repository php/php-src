<?php
class SOAP_Interop_GroupB {

    function echoStructAsSimpleTypes ($struct)
    {
      return array('outputString'  => $struct->varString,
                   'outputInteger' => $struct->varInt,
                   'outputFloat'   => $struct->varFloat);
    }

    function echoSimpleTypesAsStruct($string, $int, $float)
    {
      return (object)array("varString" => $string,
      										 "varInt"    => $int,
      										 "varFloat"  => $float);
    }

    function echoNestedStruct($struct)
    {
     return $struct;
    }

    function echo2DStringArray($ary)
    {
      return $ary;
    }

    function echoNestedArray($ary)
    {
      return $ary;
    }
}

$server = new SoapServer(dirname(__FILE__)."/round2_groupB.wsdl");
$server->setClass("SOAP_Interop_GroupB");
$server->handle($HTTP_RAW_POST_DATA);
?>
