<?php
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

define('SOAP_TEST_ACTOR_NEXT','http://schemas.xmlsoap.org/soap/actor/next');
define('SOAP_TEST_ACTOR_OTHER','http://some/other/actor');

class SOAP_Test {
    var $type = 'php';
    var $test_name = NULL;
    var $method_name = NULL;
    var $method_params = NULL;
    var $expect = NULL;
    var $expect_fault = FALSE;
    var $headers = NULL;
    var $headers_expect = NULL;
    var $result = array();
    var $show = 1;
    var $debug = 0;
    var $encoding = 'UTF-8';
    
    function SOAP_Test($methodname, $params, $expect = NULL) {
        # XXX we have to do this to make php-soap happy with NULL params
        if (!$params) $params = array();
        
        if (strchr($methodname,'(')) {
            preg_match('/(.*)\((.*)\)/',$methodname,$matches);
            $this->test_name = $methodname;
            $this->method_name = $matches[1];
        } else {
            $this->test_name = $this->method_name = $methodname;
        }
        $this->method_params = $params;
        $this->expect = $expect;
        
        // determine test type
        if ($params) {
        $v = array_values($params);
        if (gettype($v[0]) == 'object' &&
            (get_class($v[0]) == 'soapvar' || get_class($v[0]) == 'soapparam'))
            $this->type = 'soapval';
        }
    }
    
    function setResult($ok, $result, $wire, $error = '', $fault = NULL)
    {
        $this->result['success'] = $ok;
        $this->result['result'] = $result;
        $this->result['error'] = $error;
        $this->result['wire'] = $wire;
        $this->result['fault'] = $fault;
    }

    /**
    *  showMethodResult
    * print simple output about a methods result
    *
    * @param array endpoint_info
    * @param string method
    * @access public
    */    
    function showTestResult($debug = 0) {
        // debug output
        if ($debug) $this->show = 1;
        if ($debug) {
            echo str_repeat("-",50)."<br>\n";
        }
        
        echo "testing $this->test_name : ";
        if ($this->headers) {
            foreach ($this->headers as $h) {
                if (get_class($h) == 'soap_header') {
                    
                    echo "\n    {$h->name},{$h->attributes['SOAP-ENV:actor']},{$h->attributes['SOAP-ENV:mustUnderstand']} : ";
                } else {
                    if (!$h[4]) $h[4] = SOAP_TEST_ACTOR_NEXT;
                    if (!$h[3]) $h[3] = 0;
                    echo "\n    $h[0],$h[4],$h[3] : ";
                }
            }
        }
        
        if ($debug) {
            print "method params: ";
            print_r($this->params);
            print "\n";
        }
        
        $ok = $this->result['success'];
        if ($ok) {
            print "SUCCESS\n";
        } else {
            $fault = $this->result['fault'];
            if ($fault) {
                print "FAILED: {$fault->faultcode} {$fault->faultstring}\n";
            } else {
                print "FAILED: ".$this->result['result']."\n";
            }
        }
        if ($debug) {
            echo "<pre>\n".htmlentities($this->result['wire'])."</pre>\n";
        }
    }
}

# XXX I know this isn't quite right, need to deal with this better
function make_2d($x, $y)
{
    for ($_x = 0; $_x < $x; $_x++) {
        for ($_y = 0; $_y < $y; $_y++) {
            $a[$_x][$_y] = "x{$_x}y{$_y}";
        }
    }
    return $a;
}

function soap_value($name, $value, $type) {
    return new soapparam(new soapvar($value,$type),$name);
}

class SOAPStruct {
    var $varString;
    var $varInt;
    var $varFloat;
    function SOAPStruct($s, $i, $f) {
        $this->varString = $s;
        $this->varInt = $i;
        $this->varFloat = $f;
    }
}

//***********************************************************
// Base echoString

$soap_tests['base'][] = new SOAP_Test('echoString', array('inputString' => 'hello world!'));
$soap_tests['base'][] = new SOAP_Test('echoString', array('inputString' => soap_value('inputString','hello world',XSD_STRING)));
$soap_tests['base'][] = new SOAP_Test('echoString(null)', array('inputString' => ""));
$soap_tests['base'][] = new SOAP_Test('echoString(null)', array('inputString' => soap_value('inputString','',XSD_STRING)));
$soap_tests['base'][] = new SOAP_Test('echoString(entities)', array('inputString' => 'hello world\nline 2\n'));
$soap_tests['base'][] = new SOAP_Test('echoString(entities)', array('inputString' => soap_value('inputString','hello world\nline 2\n',XSD_STRING)));
$test = new SOAP_Test('echoString(utf-8)', array('inputString' => utf8_encode('ỗÈéóÒ₧⅜ỗỸ')));
$test->encoding = 'UTF-8';
$soap_tests['base'][] = $test;
$test = new SOAP_Test('echoString(utf-8)', array('inputString' => soap_value('inputString',utf8_encode('ỗÈéóÒ₧⅜ỗỸ'),XSD_STRING)));
$test->encoding = 'UTF-8';
$soap_tests['base'][] = $test;

//***********************************************************
// Base echoStringArray

$soap_tests['base'][] = new SOAP_Test('echoStringArray',
        array('inputStringArray' => array('good','bad')));
$soap_tests['base'][] = new SOAP_Test('echoStringArray',
        array('inputStringArray' =>
            soap_value('inputStringArray',array('good','bad'),SOAP_ENC_ARRAY)));
        
// null array test
# XXX NULL Arrays not supported
#$soap_tests['base'][] = new SOAP_Test('echoStringArray(null)', array('inputStringArray' => NULL));
#$soap_tests['base'][] = new SOAP_Test('echoStringArray(null)', array('inputStringArray' => soap_value('inputStringArray',NULL,XSD_STRING)));

//***********************************************************
// Base echoInteger

$soap_tests['base'][] = new SOAP_Test('echoInteger', array('inputInteger' => 34345));
$soap_tests['base'][] = new SOAP_Test('echoInteger', array('inputInteger' => soap_value('inputInteger',12345,XSD_INT)));

//***********************************************************
// Base echoIntegerArray

$soap_tests['base'][] = new SOAP_Test('echoIntegerArray', array('inputIntegerArray' => array(1,234324324,2)));
$soap_tests['base'][] = new SOAP_Test('echoIntegerArray',
        array('inputIntegerArray' =>
            soap_value('inputIntegerArray',
                       array(new soapvar(12345,XSD_INT),new soapvar(654321,XSD_INT)),
                    SOAP_ENC_ARRAY)));
#
#// null array test
# XXX NULL Arrays not supported
#$soap_tests['base'][] = new SOAP_Test('echoIntegerArray(null)', array('inputIntegerArray' => NULL));
#$soap_tests['base'][] = new SOAP_Test('echoIntegerArray(null)', array('inputIntegerArray' => new SOAP_Value('inputIntegerArray','Array',NULL)));
#
//***********************************************************
// Base echoFloat

$soap_tests['base'][] = new SOAP_Test('echoFloat', array('inputFloat' => 342.23));
$soap_tests['base'][] = new SOAP_Test('echoFloat', array('inputFloat' => soap_value('inputFloat',123.45,XSD_FLOAT)));

//***********************************************************
// Base echoFloatArray

$soap_tests['base'][] = new SOAP_Test('echoFloatArray', array('inputFloatArray' => array(1.3223,34.2,325.325)));
$soap_tests['base'][] = new SOAP_Test('echoFloatArray', 
        array('inputFloatArray' =>
            soap_value('inputFloatArray',
                       array(new soapvar(123.45,XSD_FLOAT),new soapvar(654.321,XSD_FLOAT)),
                    SOAP_ENC_ARRAY)));
//***********************************************************
// Base echoStruct

$soapstruct = new SOAPStruct('arg',34,325.325);
# XXX no way to set a namespace!!!
$soapsoapstruct = soap_value('inputStruct',$soapstruct,SOAP_ENC_OBJECT);
$soap_tests['base'][] = new SOAP_Test('echoStruct', array('inputStruct' =>$soapstruct));
$soap_tests['base'][] = new SOAP_Test('echoStruct', array('inputStruct' =>$soapsoapstruct));

//***********************************************************
// Base echoStructArray

$soap_tests['base'][] = new SOAP_Test('echoStructArray', array('inputStructArray' => array(
        $soapstruct,$soapstruct,$soapstruct)));
$soap_tests['base'][] = new SOAP_Test('echoStructArray', array('inputStructArray' =>
        soap_value('inputStructArray',array($soapstruct,$soapstruct,$soapstruct),SOAP_ENC_ARRAY)));


//***********************************************************
// Base echoVoid

$soap_tests['base'][] = new SOAP_Test('echoVoid', NULL);
$test = new SOAP_Test('echoVoid', NULL);
$test->type = 'soapval';
$soap_tests['base'][] = $test;

//***********************************************************
// Base echoBase64

$soap_tests['base'][] = new SOAP_Test('echoBase64', array('inputBase64' => 'TmVicmFza2E='));
$soap_tests['base'][] = new SOAP_Test('echoBase64', array('inputBase64' =>
        soap_value('inputBase64','TmVicmFza2E=',XSD_BASE64BINARY)));                                                          

//***********************************************************
// Base echoHexBinary

$soap_tests['base'][] = new SOAP_Test('echoHexBinary', array('inputHexBinary' => '736F61707834'));
$soap_tests['base'][] = new SOAP_Test('echoHexBinary', array('inputHexBinary' => 
        soap_value('inputHexBinary','736F61707834',XSD_HEXBINARY)));                                                          

//***********************************************************
// Base echoDecimal

# XXX test fails because php-soap incorrectly sets decimal to long rather than float
$soap_tests['base'][] = new SOAP_Test('echoDecimal', array('inputDecimal' => 12345.67890));
$soap_tests['base'][] = new SOAP_Test('echoDecimal', array('inputDecimal' => 
        soap_value('inputDecimal',12345.67890,XSD_DECIMAL)));                                                          

//***********************************************************
// Base echoDate

# php-soap doesn't handle datetime types properly yet
$soap_tests['base'][] = new SOAP_Test('echoDate', array('inputDate' => '2001-05-24T17:31:41Z'));
$soap_tests['base'][] = new SOAP_Test('echoDate', array('inputDate' => 
        soap_value('inputDate','2001-05-24T17:31:41Z',XSD_DATETIME)));#'2001-04-25T13:31:41-0700'
        
//***********************************************************
// Base echoBoolean

# php-soap sends boolean as zero or one, which is ok, but to be explicit, send true or false.
$soap_tests['base'][] = new SOAP_Test('echoBoolean', array('inputBoolean' => TRUE));
$soap_tests['base'][] = new SOAP_Test('echoBoolean', array('inputBoolean' =>
        soap_value('inputBoolean',TRUE,XSD_BOOLEAN)));
$soap_tests['base'][] = new SOAP_Test('echoBoolean', array('inputBoolean' => FALSE));
$soap_tests['base'][] = new SOAP_Test('echoBoolean', array('inputBoolean' =>
        soap_value('inputBoolean',FALSE,XSD_BOOLEAN)));

#
#
#//***********************************************************
#// GROUP B
#
#
#//***********************************************************
#// GroupB echoStructAsSimpleTypes
#
#$expect = array(
#        'outputString'=>'arg',
#        'outputInteger'=>34,
#        'outputFloat'=>325.325
#    );
#$soap_tests['GroupB'][] = new SOAP_Test('echoStructAsSimpleTypes',
#    array('inputStruct' => array(
#        'varString'=>'arg',
#        'varInt'=>34,
#        'varFloat'=>325.325
#    )), $expect);
#$soap_tests['GroupB'][] = new SOAP_Test('echoStructAsSimpleTypes',
#    array('inputStruct' =>
#        new SOAP_Value('inputStruct','SOAPStruct',
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325)
#            ))), $expect);
#
#//***********************************************************
#// GroupB echoSimpleTypesAsStruct
#
#$expect =
#    array(
#        'varString'=>'arg',
#        'varInt'=>34,
#        'varFloat'=>325.325
#    );
#$soap_tests['GroupB'][] = new SOAP_Test('echoSimpleTypesAsStruct',
#    array(
#        'inputString'=>'arg',
#        'inputInteger'=>34,
#        'inputFloat'=>325.325
#    ), $expect);
#$soap_tests['GroupB'][] = new SOAP_Test('echoSimpleTypesAsStruct',
#    array(
#        new SOAP_Value('inputString','string','arg'),
#        new SOAP_Value('inputInteger','int',34),
#        new SOAP_Value('inputFloat','float',325.325)
#    ), $expect);    
#
#//***********************************************************
#// GroupB echo2DStringArray
#
#$soap_tests['GroupB'][] = new SOAP_Test('echo2DStringArray',
#    array('input2DStringArray' => make_2d(3,3)));
#
#$multidimarray =
#    new SOAP_Value('input2DStringArray','Array',
#        array(
#            array(
#                new SOAP_Value('item','string','row0col0'),
#                new SOAP_Value('item','string','row0col1'),
#                new SOAP_Value('item','string','row0col2')
#                 ),
#            array(
#                new SOAP_Value('item','string','row1col0'),
#                new SOAP_Value('item','string','row1col1'),
#                new SOAP_Value('item','string','row1col2')
#                )
#        )
#    );
#$multidimarray->options['flatten'] = TRUE;
#$soap_tests['GroupB'][] = new SOAP_Test('echo2DStringArray',
#    array('input2DStringArray' => $multidimarray));
#
#//***********************************************************
#// GroupB echoNestedStruct
#
#$soap_tests['GroupB'][] = new SOAP_Test('echoNestedStruct',
#    array('inputStruct' => array(
#        'varString'=>'arg',
#        'varInt'=>34,
#        'varFloat'=>325.325,
#        'varStruct' => array(
#            'varString'=>'arg',
#            'varInt'=>34,
#            'varFloat'=>325.325
#        )
#    )));
#$soap_tests['GroupB'][] = new SOAP_Test('echoNestedStruct',
#    array('inputStruct' =>
#        new SOAP_Value('inputStruct','struct',
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325),
#                new SOAP_Value('varStruct','SOAPStruct',
#                    array( #push struct elements into one soap value
#                        new SOAP_Value('varString','string','arg'),
#                        new SOAP_Value('varInt','int',34),
#                        new SOAP_Value('varFloat','float',325.325)
#                    )
#                /*,NULL,'http://soapinterop.org/xsd'*/)
#            )
#        )));
#
#//***********************************************************
#// GroupB echoNestedArray
#
#$soap_tests['GroupB'][] = new SOAP_Test('echoNestedArray',
#    array('inputStruct' => array(
#        'varString'=>'arg',
#        'varInt'=>34,
#        'varFloat'=>325.325,
#        'varArray' => array('red','blue','green')
#    )));
#$soap_tests['GroupB'][] = new SOAP_Test('echoNestedArray',
#    array('inputStruct' =>
#        new SOAP_Value('inputStruct','struct',
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325),
#                new SOAP_Value('varArray','Array',
#                    array( #push struct elements into one soap value
#                        new SOAP_Value('item','string','red'),
#                        new SOAP_Value('item','string','blue'),
#                        new SOAP_Value('item','string','green')
#                    )
#                )
#            )
#        )));
#        
#
#//***********************************************************
#// GROUP C header tests
#
#//***********************************************************
#// echoMeStringRequest php val tests
#
#// echoMeStringRequest with endpoint as header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'hello world', 0,SOAP_TEST_ACTOR_NEXT);
#$test->headers_expect['echoMeStringRequest'] = array('echoMeStringResponse'=>'hello world');
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint as header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'hello world', 1,SOAP_TEST_ACTOR_NEXT);
#$this->type = 'soapval'; // force a soapval version of this test
#$test->headers_expect['echoMeStringRequest'] = array('echoMeStringResponse'=>'hello world');
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint NOT header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'hello world', 0, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStringRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint NOT header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'hello world', 1, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStringRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#//***********************************************************
#// echoMeStringRequest soapval tests
#
#// echoMeStringRequest with endpoint as header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'string', 'hello world');
#$test->headers_expect['echoMeStringRequest'] = array('echoMeStringResponse'=>'hello world');
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint as header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'string', 'hello world', 1);
#$this->type = 'soapval'; // force a soapval version of this test
#$test->headers_expect['echoMeStringRequest'] = array('echoMeStringResponse'=>'hello world');
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint NOT header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'string', 'hello world', 0, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStringRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint NOT header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStringRequest', 'string', 'hello world', 1, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStringRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStringRequest with endpoint header destination, must understand,
#// invalid namespace, should recieve a fault
##$test = new SOAP_Test('echoVoid', NULL);
##$test->type = 'soapval';
##$test->headers[] = new SOAP_Header('{http://unknown.org/echoheader/}echoMeStringRequest', 'string', 'hello world',  1);
##$test->headers_expect['echoMeStringRequest'] = array();
##$test->expect_fault = TRUE;
##$soap_tests['GroupC'][] = $test;
#
#//***********************************************************
#// php val tests
#// echoMeStructRequest with endpoint as header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStructRequest',
#        array('varString'=>'arg', 'varInt'=>34, 'varFloat'=>325.325),
#        0,SOAP_TEST_ACTOR_NEXT);
#$test->headers_expect['echoMeStructRequest'] =
#    array('echoMeStructResponse'=> array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStructRequest with endpoint as header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStructRequest',
#        array('varString'=>'arg', 'varInt'=>34, 'varFloat'=>325.325),
#        1,SOAP_TEST_ACTOR_NEXT);
#$test->headers_expect['echoMeStructRequest'] =
#    array('echoMeStructResponse'=> array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStructRequest with endpoint NOT header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStructRequest',
#        array('varString'=>'arg', 'varInt'=>34, 'varFloat'=>325.325),
#        0, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStructRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStructRequest with endpoint NOT header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeStructRequest',
#        array('varString'=>'arg', 'varInt'=>34, 'varFloat'=>325.325),
#        1, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStructRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#//***********************************************************
#// soapval tests
#// echoMeStructRequest with endpoint as header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStructRequest',NULL,
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325)
#            ));
#$test->headers_expect['echoMeStructRequest'] =
#    array('echoMeStructResponse'=> array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStructRequest with endpoint as header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStructRequest',NULL,
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325)
#            ), 1);
#$test->headers_expect['echoMeStructRequest'] =
#    array('echoMeStructResponse'=> array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStructRequest with endpoint NOT header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStructRequest',NULL,
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325)
#            ), 0, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStructRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeStructRequest with endpoint NOT header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeStructRequest',NULL,
#            array( #push struct elements into one soap value
#                new SOAP_Value('varString','string','arg'),
#                new SOAP_Value('varInt','int',34),
#                new SOAP_Value('varFloat','float',325.325)
#            ), 1, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeStructRequest'] = array();
#$soap_tests['GroupC'][] = $test;
#
#//***********************************************************
#// echoMeUnknown php val tests
#// echoMeUnknown with endpoint as header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeUnknown', 'nobody understands me!',0,SOAP_TEST_ACTOR_NEXT);
#$test->headers_expect['echoMeUnknown'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeUnknown with endpoint as header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeUnknown', 'nobody understands me!',1,SOAP_TEST_ACTOR_NEXT);
#$test->headers_expect['echoMeUnknown'] = array();
#$test->expect_fault = TRUE;
#$soap_tests['GroupC'][] = $test;
#
#// echoMeUnknown with endpoint NOT header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeUnknown', 'nobody understands me!',0, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeUnknown'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeUnknown with endpoint NOT header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->headers[] = array('{http://soapinterop.org/echoheader/}echoMeUnknown', 'nobody understands me!', 1, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeUnknown'] = array();
#$soap_tests['GroupC'][] = $test;
#
#//***********************************************************
#// echoMeUnknown soapval tests
#// echoMeUnknown with endpoint as header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeUnknown','string','nobody understands me!');
#$test->headers_expect['echoMeUnknown'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeUnknown with endpoint as header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeUnknown','string','nobody understands me!',1);
#$test->headers_expect['echoMeUnknown'] = array();
#$test->expect_fault = TRUE;
#$soap_tests['GroupC'][] = $test;
#
#// echoMeUnknown with endpoint NOT header destination, doesn't have to understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeUnknown','string','nobody understands me!', 0, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeUnknown'] = array();
#$soap_tests['GroupC'][] = $test;
#
#// echoMeUnknown with endpoint NOT header destination, must understand
#$test = new SOAP_Test('echoVoid', NULL);
#$test->type = 'soapval';
#$test->headers[] = new SOAP_Header('{http://soapinterop.org/echoheader/}echoMeUnknown','string','nobody understands me!', 1, SOAP_TEST_ACTOR_OTHER);
#$test->headers_expect['echoMeUnknown'] = array();
#$soap_tests['GroupC'][] = $test;


?>