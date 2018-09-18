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

$server = new SoapServer((isset($_SERVER['HTTPS'])?"https://":"http://").$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/interopB.wsdl.php");
$server->setClass("SOAP_Interop_GroupB");
$server->handle();
?>
