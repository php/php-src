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
// | Authors: Shane Caraveo <Shane@Caraveo.com>                           |
// +----------------------------------------------------------------------+
//
// $Id$
//

class SOAP_Interop_GroupC {
    var $method_namespace = 'http://soapinterop.org/echoheader/';

    function echoMeStringRequest($string)
    {
//        return $string;
//        return new SoapVar($string, XSD_STRING, "string", XSD_NAMESPACE, "echoMeStringResponse", $this->method_namespace);
        return new SoapHeader($this->method_namespace, "echoMeStringResponse", $string);
    }

    function echoMeStructRequest($struct)
    {
//        return $struct;
//        return new SoapVar($struct, SOAP_ENC_OBJECT, "SOAPStruct", "http://soapinterop.org/", "echoMeStructResponse",$this->method_namespace);
        return new SoapHeader($this->method_namespace, "echoMeStructResponse", $struct);
    }

    function echoVoid()
    {
    }
}

$server = new SoapServer((isset($_SERVER['HTTPS'])?"https://":"http://").$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/echoheadersvc.wsdl.php");
$server->setClass("SOAP_Interop_GroupC");
$server->handle();
?>