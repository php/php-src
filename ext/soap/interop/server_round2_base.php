<?
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Shane Caraveo <Shane@Caraveo.com>   Port to PEAR and more   |
// | Authors: Dietrich Ayala <dietrich@ganx4.com> Original Author         |
// +----------------------------------------------------------------------+
//
// $Id$
//

function generateFault($short, $long)
{
    $params = array(
        "faultcode" => "Server",
        "faultstring" => $short,
        "detail" => $long
    );

    $faultmsg  = new SOAP_Message("Fault",$params,"http://schemas.xmlsoap.org/soap/envelope/");
    return $faultmsg;
}

function hex2bin($data)
{
    $len = strlen($data);
    return pack("H" . $len, $data);
}


class SOAP_Interop_Base {
    var $method_namespace = 'http://soapinterop.org/';

    function SOAP_Interop_Base() {
	#if ($server) {
	#    $server->addToMap("echoString",array("string"),array("string"));
	#    $server->addToMap("echoStringArray",array(),array());
	#    $server->addToMap("echoInteger",array("int"),array("int"));
	#    $server->addToMap("echoIntegerArray",array(),array());
	#    $server->addToMap("echoFloat",array("float"),array("float"));
	#    $server->addToMap("echoFloatArray",array(),array());
	#    $server->addToMap("echoStruct",array(),array());
	#    $server->addToMap("echoStructArray",array(),array());
	#    $server->addToMap("echoVoid",array(),array());
	#    $server->addToMap("echoBase64",array("base64Binary"),array("base64Binary"));
	#    $server->addToMap("echoDate",array("dateTime"),array("dateTime"));
	#    $server->addToMap("echoHexBinary",array("hexBinary"),array("hexBinary"));
	#    $server->addToMap("echoDecimal",array("decimal"),array("decimal"));
	#    $server->addToMap("echoBoolean",array("boolean"),array("boolean"));
	#}
    }

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

    function echoMimeAttachment($stuff)
    {
        return new SOAP_Attachment('return','application/octet-stream',NULL,$stuff);
    }
}

$server = new SoapServer("http://test-uri");
$server->bind((isset($_SERVER['HTTPS'])?"https://":"http://").$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/interop.wsdl.php");
$server->setClass("SOAP_Interop_Base");
$server->handle();
?>