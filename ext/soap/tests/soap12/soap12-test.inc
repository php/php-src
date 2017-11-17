<?php
class Soap12test {
	public $header;

	function echoOk($x) {
  	return $x;
	}

	function echoString($inputString) {
		return $inputString;
	}

	function echoStringArray($inputStringArray) {
		return $inputStringArray;
	}

	function echoInteger($inputInteger) {
		return $inputInteger;
	}

	function echoIntegerArray($inputIntegerArray) {
		return $inputIntegerArray;
	}

	function echoFloat($inputFloat) {
		return $inputFloat;
	}

	function echoFloatArray($inputFloatArray) {
		return $inputFloatArray;
  }

	function echoStruct($x) {
  	return $x;
	}

	function echoStructArray($x) {
  	return $x;
	}

	function echoVoid() {
		return NULL;
	}

	function echoBase64($b_encoded) {
		return $b_encoded;
  }

	function echoDate($timeInstant) {
		return $timeInstant;
	}

	function echoHexBinary($hb) {
		return $hb;
	}

	function echoDecimal($dec) {
		return $dec;
	}

	function echoBoolean($boolean) {
		return $boolean;
	}

	function echoStructAsSimpleTypes ($struct) {
		 return array('outputString'  => $struct->varString,
		              'outputInteger' => $struct->varInt,
		              'outputFloat'   => $struct->varFloat);
	}

	function echoSimpleTypesAsStruct($string, $int, $float) {
		return (object)array("varString" => $string,
		 										 "varInt"    => $int,
	  										 "varFloat"  => $float);
	}

	function echoNestedStruct($struct) {
		return $struct;
	}

	function echo2DStringArray($ary) {
		return $ary;
	}

	function echoNestedArray($ary) {
		return $ary;
	}

	function countItems($input) {
		return count($input);
	}

	function isNil($input=NULL) {
		return is_null($input);
	}

	function returnVoid() {
	}

	function emptyBody() {
	}

	function requiredHeader($x) {
		$this->header = $x;
	}

	function echoHeader() {
	  return $this->header;
	}

	function echoResolvedRef($ref) {
	  return $ref->RelativeReference->base.$ref->RelativeReference->href;
	}

	function validateCountryCode($code) {
		if (strlen($code) != 2) {
		  return new SoapFault("Client", "Not a valid country code", NULL, NULL, NULL, new SoapHeader("http://example.org/ts-tests", "validateCountryCodeFault", "Country code must be 2 letters."));
		} else {
			return "OK";
		}
	}

}

ini_set("soap.wsdl_cache_enabled",0);
$server = new soapserver(dirname(__FILE__)."/soap12-test.wsdl", array('soap_version'=>SOAP_1_2,'actor'=>"http://example.org/ts-tests/C"));
$server->setClass("Soap12test");

$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
