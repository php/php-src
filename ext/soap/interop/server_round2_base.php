<?
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2018 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at                              |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Shane Caraveo <Shane@Caraveo.com>   Port to PEAR and more   |
// | Authors: Dietrich Ayala <dietrich@ganx4.com> Original Author         |
// +----------------------------------------------------------------------+

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

    function echoMimeAttachment($stuff)
    {
        return new SOAP_Attachment('return','application/octet-stream',NULL,$stuff);
    }
}

$server = new SoapServer((isset($_SERVER['HTTPS'])?"https://":"http://").$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/interop.wsdl.php");
$server->setClass("SOAP_Interop_Base");
$server->handle();
?>
