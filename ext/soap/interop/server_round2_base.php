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
require_once 'SOAP/Server.php';

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
	return new SOAP_Value('outputString','string',$inputString);
    }

    function echoStringArray($inputStringArray)
    {
	$ra = array();
	if ($inputStringArray) {
	foreach($inputStringArray as $s) {
	    $ra[] = new SOAP_Value('item','string',$s);
	}
	}
	return new SOAP_Value('outputStringArray',NULL,$ra);
    }


    function echoInteger($inputInteger)
    {
	return new SOAP_Value('outputInteger','int',(integer)$inputInteger);
    }

    function echoIntegerArray($inputIntegerArray)
    {
	$ra = array();
	if ($inputIntegerArray) {
	foreach ($inputIntegerArray as $i) {
	    $ra[] = new SOAP_Value('item','int',$i);
	}
	}
	return new SOAP_Value('outputIntArray',NULL,$ra);
    }

    function echoFloat($inputFloat)
    {
	return new SOAP_Value('outputFloat','float',(FLOAT)$inputFloat);
    }

    function echoFloatArray($inputFloatArray)
    {
	$ra = array();
	if ($inputFloatArray) {
	foreach($inputFloatArray as $float) {
	    $ra[] = new SOAP_Value('item','float',(FLOAT)$float);
	}
	}
	return new SOAP_Value('outputFloatArray',NULL,$ra);
    }

    function echoStruct($inputStruct)
    {
	return new SOAP_Value('return','{http://soapinterop.org/xsd}SOAPStruct',
			      array(
				  new SOAP_Value('varInt','int',$inputStruct['varInt']),
				  new SOAP_Value('varFloat','float',$inputStruct['varFloat']),
				  new SOAP_Value('varString','string',$inputStruct['varString'])
				   ));
    }

    function echoStructArray($inputStructArray)
    {
	$ra = array();
	if ($inputStructArray) {
	foreach($inputStructArray as $struct) {
	    $ra[] = new SOAP_Value('item','{http://soapinterop.org/xsd}SOAPStruct',
			      array(
				  new SOAP_Value('varInt','int',$struct['varInt']),
				  new SOAP_Value('varFloat','float',$struct['varFloat']),
				  new SOAP_Value('varString','string',$struct['varString'])
				   ));
	}
	}
	return $ra;
    }

    function echoVoid()
    {
	return NULL;
    }

    function echoBase64($b_encoded)
    {
	return new SOAP_Value('return','base64Binary',base64_encode(base64_decode($b_encoded)));
    }

    function echoDate($timeInstant)
    {
	$dt = new SOAP_Type_dateTime($timeInstant);
	if ($dt->toUnixtime() != -1) {
	    $value = $dt->toSOAP();
	    return new SOAP_Value('return','dateTime',$value);
	} else {
	    return new SOAP_Fault("Value $timeInstant is not a dateTime value");
	}
    }

    function echoHexBinary($hb)
    {
	return new SOAP_Value('return','hexBinary',bin2hex(hex2bin($hb)));
    }

    function echoDecimal($dec)
    {
	return new SOAP_Value('return','decimal',(FLOAT)$dec);
    }

    function echoBoolean($boolean)
    {
	return new SOAP_Value('return','boolean',$boolean);
    }
    
    function echoMimeAttachment($stuff)
    {
        return new SOAP_Attachment('return','application/octet-stream',NULL,$stuff);
    }
}

$base = new SOAP_Interop_Base();
$server->addObjectMap($base);
?>