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

class SOAP_Interop_GroupB {
    var $method_namespace = 'http://soapinterop.org/';
    var $dispatch_map = array();
    
    function SOAP_Interop_GroupB() {
	$this->dispatch_map['echoStructAsSimpleTypes'] =
		array('in' => array('inputStruct' => 'SOAPStruct'),
		      'out' => array('outputString' => 'string', 'outputInteger' => 'int', 'outputFloat' => 'float')
		      );
	
#        $server->addToMap('echoSimpleTypesAsStruct',
#		      array('outputString' => 'string', 'outputInteger' => 'int', 'outputFloat' => 'float'),
#		      array('return' => 'struct'));
#	$server->addToMap('echoNestedStruct', array(), array());
#	$server->addToMap('echo2DStringArray', array(), array());
#	$server->addToMap('echoNestedArray', array(), array());
    }
    function echoStructAsSimpleTypes ($struct)
    {
	# convert a SOAPStruct to an array
	$vals = array_values($struct);
	return array(
	    new SOAP_Value('outputString','string',$struct['varString']),
	    new SOAP_Value('outputInteger','int',$struct['varInt']),
	    new SOAP_Value('outputFloat','float',$struct['varFloat'])
	    );
	return array_values($struct);
    }

    function echoSimpleTypesAsStruct($string, $int, $float)
    {
	# convert a input into struct
	$ret = new SOAP_Value('return','{http://soapinterop.org/xsd}SOAPStruct',
		array( #push struct elements into one soap value
		    new SOAP_Value('varString','string',$string),
		    new SOAP_Value('varInt','int',(int)$int),
		    new SOAP_Value('varFloat','float',(FLOAT)$float)
		)
	    );
	return $ret;
    }

    function echoNestedStruct($struct)
    {
	return $struct;
    }

    function echo2DStringArray($ary)
    {
	$ret = new SOAP_Value('return','Array',$ary);
	$ret->options['flatten'] = TRUE;
	return $ret;
    }

    function echoNestedArray($ary)
    {
	return $ary;
    }
}

$groupb = new SOAP_Interop_GroupB();
$server->addObjectMap($groupb);

?>