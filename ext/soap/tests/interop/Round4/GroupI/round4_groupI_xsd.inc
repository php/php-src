<?php
class SOAP_Interop_GroupI {

    function echoString($inputString)
    {
    	if (isset($inputString->inputString)) {
	      return array("return"=>$inputString->inputString);
	    } else {
	      return $inputString;
	    }
    }

    function echoInteger($inputInteger)
    {
      return array("return"=>$inputInteger->inputInteger);
    }

    function echoFloat($inputFloat)
    {
      return array("return"=>$inputFloat->inputFloat);
    }

    function echoVoid()
    {
    }

    function echoBase64($inputBase64)
    {
    	if (isset($inputBase64->inputBase64)) {
	      return array("return"=>$inputBase64->inputBase64);
	    } else {
	      return $inputBase64;
	    }
    }

    function echoDate($timeInstant)
    {
      return array("return"=>$timeInstant->inputDate);
    }

    function echoComplexType($inputComplexType)
    {
    	if (isset($inputComplexType->inputComplexType)) {
	      return array("return"=>$inputComplexType->inputComplexType);
	    } else {
	      return $inputComplexType;
	    }
    }

    function echoIntegerMultiOccurs($input)
    {
    	if (isset($input->inputIntegerMultiOccurs->int)) {
	      return array("return"=>$input->inputIntegerMultiOccurs->int);
	    } else {
	      return array();
	    }
    }

    function echoFloatMultiOccurs($input)
    {
    	if (isset($input->inputFloatMultiOccurs->float)) {
	      return array("return"=>$input->inputFloatMultiOccurs->float);
	    } else {
	      return array();
	    }
    }

    function echoStringMultiOccurs($input)
    {
    	if (isset($input->inputStringMultiOccurs->string)) {
	      return array("return"=>$input->inputStringMultiOccurs->string);
	    } else {
	      return array();
	    }
    }

    function echoComplexTypeMultiOccurs($input)
    {
    	if (isset($input->inputComplexTypeMultiOccurs->SOAPComplexType)) {
	      return array("return"=>$input->inputComplexTypeMultiOccurs->SOAPComplexType);
	    } else {
	      return array();
	    }
    }

    function echoHexBinary($hb)
    {
    	if (isset($hb->inputHexBinary)) {
	      return array("return"=>$hb->inputHexBinary);
	    } else {
	      return $hb;
	    }
    }

    function echoDecimal($dec)
    {
      return array("return"=>$dec->inputDecimal);
    }

    function echoBoolean($boolean)
    {
      return array("return"=>$boolean->inputBoolean);
    }

    function echoComplexTypeAsSimpleTypes($input)
    {
    	if (isset($input->inputComplexType)) {
    	  $ret = array("outputInteger" => $input->inputComplexType->varInt,
    	               "outputFloat"   => $input->inputComplexType->varFloat);
    	  if (isset($input->inputComplexType->varString)) {
    	    $ret["outputString"] = $input->inputComplexType->varString;
    	  }
	      return $ret;
	    } else {
	      return array();
	    }
	  }

    function echoSimpleTypesAsComplexType($input)
    {
   	  $ret = array("varInt"   => $input->inputInteger,
   	               "varFloat" => $input->inputFloat);
   	  if (isset($input->inputString)) {
   	    $ret["varString"] = $input->inputString;
   	  }
   	  return array("return"=>$ret);
    }

    function echoNestedComplexType($inputComplexType)
    {
    	if (isset($inputComplexType->inputComplexType)) {
	      return array("return"=>$inputComplexType->inputComplexType);
	    } else {
	      return $inputComplexType;
	    }
    }

    function echoNestedMultiOccurs($inputComplexType)
    {
    	if (isset($inputComplexType->inputComplexType)) {
	      return array("return"=>$inputComplexType->inputComplexType);
	    } else {
	      return $inputComplexType;
	    }
    }

    function echoChoice($input)
    {
    	if (isset($input->inputChoice)) {
	      return array("return"=>$input->inputChoice);
	    } else {
	      return $input;
	    }
    }

    function echoEnum($input)
    {
      return array("return"=>$input->inputEnum);
    }

    function echoAnyType($input)
    {
    	if (isset($input->inputAnyType)) {
	      return array("return"=>$input->inputAnyType);
	    } else {
	      return $input;
	    }
    }

    function echoAnyElement($input)
    {
    	if (isset($input->inputAny)) {
	      return array("return"=>$input->inputAny);
	    } else {
	      return $input;
	    }
    }

    function echoVoidSoapHeader()
    {
    }

    function echoMeStringRequest($input)
    {
      return new SoapHeader("http://soapinterop.org/","echoMeStringResponse",$input);
    }

    function echoMeComplexTypeRequest($input)
    {
      return new SoapHeader("http://soapinterop.org/","echoMeComplexTypeResponse",$input);
    }

}

$server = new SoapServer(dirname(__FILE__)."/round4_groupI_xsd.wsdl");
$server->setClass("SOAP_Interop_GroupI");
$server->handle($HTTP_RAW_POST_DATA);
?>
