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
require_once 'SOAP/Server.php';
require_once 'SOAP/Value.php';

class SOAP_Interop_GroupC {
    var $method_namespace = 'http://soapinterop.org/echoheader/';
    
    function echoMeStringRequest($string)
    {
	return new SOAP_Value('{'.$this->method_namespace.'}echoMeStringResponse','string',$string);
    }

    function echoMeStructRequest($struct)
    {
	return new SOAP_Value('{'.$this->method_namespace.'}echoMeStructResponse','SOAPStruct',$struct);
    }
}

$groupc = new SOAP_Interop_GroupC();
$server->addObjectMap($groupc);

?>