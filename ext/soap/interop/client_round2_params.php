<?php
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
// | Authors: Shane Caraveo <Shane@Caraveo.com>                           |
// +----------------------------------------------------------------------+

define('SOAP_TEST_ACTOR_OTHER','http://some/other/actor');

class SOAP_Test {
    var $type = 'php';
    var $test_name = NULL;
    var $method_name = NULL;
    var $method_params = NULL;
    var $cmp_func = NULL;
    var $expect = NULL;
    var $expect_fault = FALSE;
    var $headers = NULL;
    var $headers_expect = NULL;
    var $result = array();
    var $show = 1;
    var $debug = 0;
    var $encoding = 'UTF-8';

    function SOAP_Test($methodname, $params, $expect = NULL, $cmp_func = NULL) {
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
        if ($expect !== NULL) {
          $this->expect = $expect;
        }
        if ($cmp_func !== NULL) {
          $this->cmp_func = $cmp_func;
        }

        // determine test type
        if ($params) {
        $v = array_values($params);
        if (gettype($v[0]) == 'object' &&
            (get_class($v[0]) == 'SoapVar' || get_class($v[0]) == 'SoapParam'))
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
    function showTestResult($debug = 0, $html = 0) {
        // debug output
        if ($debug) $this->show = 1;
        if ($debug) {
            echo str_repeat("-",50).$html?"<br>\n":"\n";
        }

        echo "testing $this->test_name : ";

        if ($debug) {
            print "method params: ";
            print_r($this->params);
            print "\n";
        }

        $ok = $this->result['success'];
        if ($ok) {
            if ($html) {
                print "<font color=\"#00cc00\">SUCCESS</font>\n";
            } else {
                print "SUCCESS\n";
            }
        } else {
            $fault = $this->result['fault'];
            if ($fault) {
            		$res = $fault->faultcode;
                $pos = strpos($res,':');
                if ($pos !== false) {
                	$res = substr($res,$pos+1);
                }
                if ($html) {
                    print "<font color=\"#ff0000\">FAILED: [$res] {$fault->faultstring}</font>\n";
                } else {
                    print "FAILED: [$res] {$fault->faultstring}\n";
                }
            } else {
                if ($html) {
                    print "<font color=\"#ff0000\">FAILED: ".$this->result['result']."</font>\n";
                } else {
                    print "FAILED: ".$this->result['result']."\n";
                }
            }
        }
        if ($debug) {
            if ($html) {
                echo "<pre>\n".htmlentities($this->result['wire'])."</pre>\n";
            } else {
                echo "\n".htmlentities($this->result['wire'])."\n";
            }
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

function soap_value($name, $value, $type, $type_name=NULL, $type_ns=NULL) {
    return new SoapParam(new SoapVar($value,$type,$type_name,$type_ns),$name);
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
$soap_tests['base'][] = new SOAP_Test('echoString(empty)', array('inputString' => ''));
$soap_tests['base'][] = new SOAP_Test('echoString(empty)', array('inputString' => soap_value('inputString','',XSD_STRING)));
$soap_tests['base'][] = new SOAP_Test('echoString(null)', array('inputString' => NULL));
$soap_tests['base'][] = new SOAP_Test('echoString(null)', array('inputString' => soap_value('inputString',NULL,XSD_STRING)));
//$soap_tests['base'][] = new SOAP_Test('echoString(entities)', array('inputString' => ">,<,&,\",',0:\x00",1:\x01,2:\x02,3:\x03,4:\x04,5:\x05,6:\x06,7:\x07,8:\x08,9:\x09,10:\x0a,11:\x0b,12:\x0c,13:\x0d,14:\x0e,15:\x0f,16:\x10,17:\x11,18:\x12,19:\x13,20:\x14,21:\x15,22:\x16,23:\x17,24:\x18,25:\x19,26:\x1a,27:\x1b,28:\x1c,29:\x1d,30:\x1e,31:\x1f"));
//$soap_tests['base'][] = new SOAP_Test('echoString(entities)', array('inputString' => soap_value('inputString',">,<,&,\",',0:\x00",1:\x01,2:\x02,3:\x03,4:\x04,5:\x05,6:\x06,7:\x07,8:\x08,9:\x09,10:\x0a,11:\x0b,12:\x0c,13:\x0d,14:\x0e,15:\x0f,16:\x10,17:\x11,18:\x12,19:\x13,20:\x14,21:\x15,22:\x16,23:\x17,24:\x18,25:\x19,26:\x1a,27:\x1b,28:\x1c,29:\x1d,30:\x1e,31:\x1f",XSD_STRING)));
$soap_tests['base'][] = new SOAP_Test('echoString(entities)', array('inputString' => ">,<,&,\",',\\,\n"));
$soap_tests['base'][] = new SOAP_Test('echoString(entities)', array('inputString' => soap_value('inputString',">,<,&,\",',\\,\n",XSD_STRING)));
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
            soap_value('inputStringArray',array('good','bad'),SOAP_ENC_ARRAY,"ArrayOfstring","http://soapinterop.org/xsd")));

$soap_tests['base'][] = new SOAP_Test('echoStringArray(one)',
        array('inputStringArray' => array('good')));
$soap_tests['base'][] = new SOAP_Test('echoStringArray(one)',
        array('inputStringArray' =>
            soap_value('inputStringArray',array('good'),SOAP_ENC_ARRAY,"ArrayOfstring","http://soapinterop.org/xsd")));

// empty array test
$soap_tests['base'][] = new SOAP_Test('echoStringArray(empty)', array('inputStringArray' => array()));
$soap_tests['base'][] = new SOAP_Test('echoStringArray(empty)', array('inputStringArray' => soap_value('inputStringArray',array(),SOAP_ENC_ARRAY,"ArrayOfstring","http://soapinterop.org/xsd")));

# XXX NULL Arrays not supported
// null array test
$soap_tests['base'][] = new SOAP_Test('echoStringArray(null)', array('inputStringArray' => NULL));
$soap_tests['base'][] = new SOAP_Test('echoStringArray(null)', array('inputStringArray' => soap_value('inputStringArray',NULL,SOAP_ENC_ARRAY,"ArrayOfstring","http://soapinterop.org/xsd")));

//***********************************************************
// Base echoInteger
$x = new SOAP_Test('echoInteger', array('inputInteger' => 34345));
$soap_tests['base'][] = new SOAP_Test('echoInteger', array('inputInteger' => 34345));
$soap_tests['base'][] = new SOAP_Test('echoInteger', array('inputInteger' => soap_value('inputInteger',12345,XSD_INT)));

//***********************************************************
// Base echoIntegerArray

$soap_tests['base'][] = new SOAP_Test('echoIntegerArray', array('inputIntegerArray' => array(1,234324324,2)));
$soap_tests['base'][] = new SOAP_Test('echoIntegerArray',
        array('inputIntegerArray' =>
            soap_value('inputIntegerArray',
                       array(new SoapVar(12345,XSD_INT),new SoapVar(654321,XSD_INT)),
                    SOAP_ENC_ARRAY,"ArrayOfint","http://soapinterop.org/xsd")));

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
                       array(new SoapVar(123.45,XSD_FLOAT),new SoapVar(654.321,XSD_FLOAT)),
                    SOAP_ENC_ARRAY,"ArrayOffloat","http://soapinterop.org/xsd")));
//***********************************************************
// Base echoStruct

$soapstruct = new SOAPStruct('arg',34,325.325);
# XXX no way to set a namespace!!!
$soapsoapstruct = soap_value('inputStruct',$soapstruct,SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd");
$soap_tests['base'][] = new SOAP_Test('echoStruct', array('inputStruct' =>$soapstruct));
$soap_tests['base'][] = new SOAP_Test('echoStruct', array('inputStruct' =>$soapsoapstruct));

//***********************************************************
// Base echoStructArray

$soap_tests['base'][] = new SOAP_Test('echoStructArray', array('inputStructArray' => array(
        $soapstruct,$soapstruct,$soapstruct)));
$soap_tests['base'][] = new SOAP_Test('echoStructArray', array('inputStructArray' =>
         soap_value('inputStructArray',array(
           new SoapVar($soapstruct,SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd"),
           new SoapVar($soapstruct,SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd"),
           new SoapVar($soapstruct,SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd")),
         SOAP_ENC_ARRAY,"ArrayOfSOAPStruct","http://soapinterop.org/xsd")));


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

$soap_tests['base'][] = new SOAP_Test('echoHexBinary', array('inputHexBinary' => '736F61707834'),'736F61707834','hex_compare');
$soap_tests['base'][] = new SOAP_Test('echoHexBinary', array('inputHexBinary' =>
        soap_value('inputHexBinary','736F61707834',XSD_HEXBINARY)),'736F61707834','hex_compare');

//***********************************************************
// Base echoDecimal

# XXX test fails because php-soap incorrectly sets decimal to long rather than float
$soap_tests['base'][] = new SOAP_Test('echoDecimal', array('inputDecimal' => '12345.67890'));
$soap_tests['base'][] = new SOAP_Test('echoDecimal', array('inputDecimal' =>
        soap_value('inputDecimal','12345.67890',XSD_DECIMAL)));

//***********************************************************
// Base echoDate

# php-soap doesn't handle datetime types properly yet
$soap_tests['base'][] = new SOAP_Test('echoDate', array('inputDate' => '2001-05-24T17:31:41Z'), null, 'date_compare');
$soap_tests['base'][] = new SOAP_Test('echoDate', array('inputDate' =>
        soap_value('inputDate','2001-05-24T17:31:41Z',XSD_DATETIME)), null, 'date_compare');

//***********************************************************
// Base echoBoolean

# php-soap sends boolean as zero or one, which is ok, but to be explicit, send true or false.
$soap_tests['base'][] = new SOAP_Test('echoBoolean(true)', array('inputBoolean' => TRUE));
$soap_tests['base'][] = new SOAP_Test('echoBoolean(true)', array('inputBoolean' =>
        soap_value('inputBoolean',TRUE,XSD_BOOLEAN)));
$soap_tests['base'][] = new SOAP_Test('echoBoolean(false)', array('inputBoolean' => FALSE));
$soap_tests['base'][] = new SOAP_Test('echoBoolean(false)', array('inputBoolean' =>
        soap_value('inputBoolean',FALSE,XSD_BOOLEAN)));
$soap_tests['base'][] = new SOAP_Test('echoBoolean(1)', array('inputBoolean' => 1),true);
$soap_tests['base'][] = new SOAP_Test('echoBoolean(1)', array('inputBoolean' =>
        soap_value('inputBoolean',1,XSD_BOOLEAN)),true);
$soap_tests['base'][] = new SOAP_Test('echoBoolean(0)', array('inputBoolean' => 0),false);
$soap_tests['base'][] = new SOAP_Test('echoBoolean(0)', array('inputBoolean' =>
        soap_value('inputBoolean',0,XSD_BOOLEAN)),false);



//***********************************************************
// GROUP B


//***********************************************************
// GroupB echoStructAsSimpleTypes

$expect = array(
        'outputString'=>'arg',
        'outputInteger'=>34,
        'outputFloat'=>325.325
    );
$soap_tests['GroupB'][] = new SOAP_Test('echoStructAsSimpleTypes',
    array('inputStruct' => (object)array(
        'varString'=>'arg',
        'varInt'=>34,
        'varFloat'=>325.325
    )), $expect);
$soap_tests['GroupB'][] = new SOAP_Test('echoStructAsSimpleTypes',
    array('inputStruct' =>
          soap_value('inputStruct',
            (object)array('varString' => 'arg',
                					'varInt'    => 34,
                          'varFloat'  => 325.325
            ), SOAP_ENC_OBJECT, "SOAPStruct","http://soapinterop.org/xsd")), $expect);

//***********************************************************
// GroupB echoSimpleTypesAsStruct

$expect =
    (object)array(
        'varString'=>'arg',
        'varInt'=>34,
        'varFloat'=>325.325
    );
$soap_tests['GroupB'][] = new SOAP_Test('echoSimpleTypesAsStruct',
    array(
        'inputString'=>'arg',
        'inputInteger'=>34,
        'inputFloat'=>325.325
    ), $expect);
$soap_tests['GroupB'][] = new SOAP_Test('echoSimpleTypesAsStruct',
    array(
        soap_value('inputString','arg', XSD_STRING),
        soap_value('inputInteger',34, XSD_INT),
        soap_value('inputFloat',325.325, XSD_FLOAT)
    ), $expect);

//***********************************************************
// GroupB echo2DStringArray

$soap_tests['GroupB'][] = new SOAP_Test('echo2DStringArray',
    array('input2DStringArray' => make_2d(3,3)));

$multidimarray =
    soap_value('input2DStringArray',
        array(
            array('row0col0', 'row0col1', 'row0col2'),
            array('row1col0', 'row1col1', 'row1col2')
        ), SOAP_ENC_ARRAY
    );
//$multidimarray->options['flatten'] = TRUE;
$soap_tests['GroupB'][] = new SOAP_Test('echo2DStringArray',
    array('input2DStringArray' => $multidimarray));

//***********************************************************
// GroupB echoNestedStruct

$strstr = (object)array(
        'varString'=>'arg',
        'varInt'=>34,
        'varFloat'=>325.325,
        'varStruct' => (object)array(
            'varString'=>'arg',
            'varInt'=>34,
            'varFloat'=>325.325
        ));
$soap_tests['GroupB'][] = new SOAP_Test('echoNestedStruct',
    array('inputStruct' => $strstr));
$soap_tests['GroupB'][] = new SOAP_Test('echoNestedStruct',
    array('inputStruct' =>
        soap_value('inputStruct',
						(object)array(
				        'varString'=>'arg',
        				'varInt'=>34,
				        'varFloat'=>325.325,
        				'varStruct' => new SoapVar((object)array(
				            'varString'=>'arg',
        				    'varInt'=>34,
				            'varFloat'=>325.325
        				), SOAP_ENC_OBJECT, "SOAPStruct","http://soapinterop.org/xsd")
            ), SOAP_ENC_OBJECT, "SOAPStructStruct","http://soapinterop.org/xsd"
        )),$strstr);

//***********************************************************
// GroupB echoNestedArray

$arrstr = (object)array(
        'varString'=>'arg',
        'varInt'=>34,
        'varFloat'=>325.325,
        'varArray' => array('red','blue','green')
    );
$soap_tests['GroupB'][] = new SOAP_Test('echoNestedArray',
    array('inputStruct' => $arrstr));
$soap_tests['GroupB'][] = new SOAP_Test('echoNestedArray',
    array('inputStruct' =>
        soap_value('inputStruct',
            (object)array('varString' => 'arg',
                          'varInt'    => 34,
                          'varFloat'  => 325.325,
                          'varArray'  =>
                    new SoapVar(array("red", "blue", "green"), SOAP_ENC_ARRAY, "ArrayOfstring","http://soapinterop.org/xsd")
                ), SOAP_ENC_OBJECT, "SOAPArrayStruct","http://soapinterop.org/xsd"
        )),$arrstr);


//***********************************************************
// GROUP C header tests

//***********************************************************
// echoMeStringRequest

// echoMeStringRequest with endpoint as header destination, doesn't have to understand
$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=0 actor=next)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', 'hello world', 0, SOAP_ACTOR_NEXT);
$test->headers_expect = array('echoMeStringResponse'=>'hello world');
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=0 actor=next)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', new SoapVar('hello world',XSD_STRING), 0, SOAP_ACTOR_NEXT);
$test->headers_expect = array('echoMeStringResponse'=>'hello world');
$soap_tests['GroupC'][] = $test;

// echoMeStringRequest with endpoint as header destination, must understand
$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=1 actor=next)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', 'hello world', 1, SOAP_ACTOR_NEXT);
$test->headers_expect = array('echoMeStringResponse'=>'hello world');
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=1 actor=next)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', new SoapVar('hello world',XSD_STRING), 1, SOAP_ACTOR_NEXT);
$test->headers_expect = array('echoMeStringResponse'=>'hello world');
$soap_tests['GroupC'][] = $test;

// echoMeStringRequest with endpoint NOT header destination, doesn't have to understand
$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=0 actor=other)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', 'hello world', 0, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=0 actor=other)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', new SoapVar('hello world',XSD_STRING), 0, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

// echoMeStringRequest with endpoint NOT header destination, must understand
$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=1 actor=other)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', 'hello world', 1, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStringRequest mustUnderstand=1 actor=other)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStringRequest', new SoapVar('hello world', XSD_STRING), 1, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

// echoMeStringRequest with endpoint header destination, must understand,
// invalid namespace, should receive a fault
$test = new SOAP_Test('echoVoid(echoMeStringRequest invalid namespace)', NULL);
$test->headers[] = new SoapHeader('http://unknown.org/echoheader/','echoMeStringRequest', 'hello world', 1, SOAP_ACTOR_NEXT);
$test->headers_expect = array();
$test->expect_fault = TRUE;
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStringRequest invalid namespace)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://unknown.org/echoheader/','echoMeStringRequest', new SoapVar('hello world', XSD_STRING), 1, SOAP_ACTOR_NEXT);
$test->headers_expect = array();
$test->expect_fault = TRUE;
$soap_tests['GroupC'][] = $test;

//***********************************************************
// echoMeStructRequest

// echoMeStructRequest with endpoint as header destination, doesn't have to understand
$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=0 actor=next)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
        new SOAPStruct('arg',34,325.325), 0, SOAP_ACTOR_NEXT);
$test->headers_expect =
    array('echoMeStructResponse'=> (object)array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=0 actor=next)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
            new SoapVar(new SOAPStruct('arg',34,325.325),SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd"),
            0, SOAP_ACTOR_NEXT);
$test->headers_expect =
    array('echoMeStructResponse'=> (object)array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
$soap_tests['GroupC'][] = $test;

// echoMeStructRequest with endpoint as header destination, must understand
$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=1 actor=next)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
        new SOAPStruct('arg',34,325.325), 1, SOAP_ACTOR_NEXT);
$test->headers_expect =
    array('echoMeStructResponse'=> (object)array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=1 actor=next)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
            new SoapVar(new SOAPStruct('arg',34,325.325),SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd"),
            1, SOAP_ACTOR_NEXT);
$test->headers_expect =
    array('echoMeStructResponse'=> (object)array('varString'=>'arg','varInt'=>34,'varFloat'=>325.325));
$soap_tests['GroupC'][] = $test;

// echoMeStructRequest with endpoint NOT header destination, doesn't have to understand
$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=0 actor=other)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
        new SOAPStruct('arg',34,325.325), 0, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=0 actor=other)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
            new SoapVar(new SOAPStruct('arg',34,325.325),SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd"),
            0, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

// echoMeStructRequest with endpoint NOT header destination, must understand
$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=1 actor=other)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
        new SOAPStruct('arg',34,325.325), 1, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeStructRequest mustUnderstand=1 actor=other)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeStructRequest',
            new SoapVar(new SOAPStruct('arg',34,325.325),SOAP_ENC_OBJECT,"SOAPStruct","http://soapinterop.org/xsd"),
            1, SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

//***********************************************************
// echoMeUnknown

// echoMeUnknown with endpoint as header destination, doesn't have to understand
$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=0 actor=next)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', 'nobody understands me!',0,SOAP_ACTOR_NEXT);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=0 actor=next)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', new SoapVar('nobody understands me!',XSD_STRING),0,SOAP_ACTOR_NEXT);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

// echoMeUnknown with endpoint as header destination, must understand
$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=1 actor=next)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', 'nobody understands me!',1,SOAP_ACTOR_NEXT);
$test->headers_expect = array();
$test->expect_fault = TRUE;
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=1 actor=next)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', new SoapVar('nobody understands me!',XSD_STRING),1,SOAP_ACTOR_NEXT);
$test->headers_expect = array();
$test->expect_fault = TRUE;
$soap_tests['GroupC'][] = $test;

// echoMeUnknown with endpoint NOT header destination, doesn't have to understand
$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=0 actor=other)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', 'nobody understands me!',0,SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=0 actor=other)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', new SoapVar('nobody understands me!',XSD_STRING),0,SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

// echoMeUnknown with endpoint NOT header destination, must understand
$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=1 actor=other)', NULL);
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', 'nobody understands me!',1,SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;

$test = new SOAP_Test('echoVoid(echoMeUnknown mustUnderstand=1 actor=other)', NULL);
$test->type = 'soapval';
$test->headers[] = new SoapHeader('http://soapinterop.org/echoheader/','echoMeUnknown', new SoapVar('nobody understands me!',XSD_STRING),1,SOAP_TEST_ACTOR_OTHER);
$test->headers_expect = array();
$soap_tests['GroupC'][] = $test;
?>
