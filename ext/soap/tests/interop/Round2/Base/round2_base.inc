<?php
class SOAP_Interop_Base {

    function echoString($inputString)
    {
      return $inputString;
    }

    function echoStringArray($inputStringArray)
    {
      return $inputStringArray;
    }


    function echoInteger($inputInteger)
    {
      return $inputInteger;
    }

    function echoIntegerArray($inputIntegerArray)
    {
      return $inputIntegerArray;
    }

    function echoFloat($inputFloat)
    {
      return $inputFloat;
    }

    function echoFloatArray($inputFloatArray)
    {
      return $inputFloatArray;
    }

    function echoStruct($inputStruct)
    {
      return $inputStruct;
    }

    function echoStructArray($inputStructArray)
    {
      return $inputStructArray;
    }

    function echoVoid()
    {
      return NULL;
    }

    function echoBase64($b_encoded)
    {
      return $b_encoded;
    }

    function echoDate($timeInstant)
    {
      return $timeInstant;
    }

    function echoHexBinary($hb)
    {
      return $hb;
    }

    function echoDecimal($dec)
    {
      return $dec;
    }

    function echoBoolean($boolean)
    {
      return $boolean;
    }

}

ini_set("soap.wsdl_cache_enabled",0);
$server = new SoapServer(dirname(__FILE__)."/round2_base.wsdl");
$server->setClass("SOAP_Interop_Base");
$server->handle($HTTP_RAW_POST_DATA);
?>