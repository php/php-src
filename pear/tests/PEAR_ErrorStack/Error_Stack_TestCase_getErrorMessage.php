<?php

/**
 * API Unit tests for PEAR_ErrorStack package.
 * 
 * @version    $Id$
 * @author     Laurent Laville <pear@laurent-laville.org> portions from HTML_CSS
 * @author     Greg Beaver
 * @package    PEAR_ErrorStack
 */

class testgemessage 
{
    function __toString()
    {
        return '__toString() called';
    }
}
class testgemessage1 {} 
/**
 * @package PEAR_ErrorStack
 */

class PEAR_ErrorStack_TestCase_getErrorMessage extends PHPUnit_TestCase
{

    function PEAR_ErrorStack_TestCase_getErrorMessage($name)
    {
        $this->PHPUnit_TestCase($name);
    }

    function setUp()
    {
        error_reporting(E_ALL);
        $this->errorOccured = false;
        set_error_handler(array(&$this, 'errorHandler'));
        $this->stack = new PEAR_ErrorStack('test');
        $s = &PEAR_ErrorStack::singleton('PEAR_ErrorStack');
        $s->pushCallback(array('PEAR_ErrorStack', '_handleError'));
    }

    function tearDown()
    {
        unset($this->stack);
        unset($GLOBALS['_PEAR_ERRORSTACK_SINGLETON']);
    }


    function _stripWhitespace($str)
    {
        return preg_replace('/\\s+/', '', $str);
    }

    function _methodExists($name) 
    {
        if (in_array(strtolower($name), get_class_methods($this->stack))) {
            return true;
        }
        $this->assertTrue(false, 'method '. $name . ' not implemented in ' . get_class($this->stack));
        return false;
    }

    function errorHandler($errno, $errstr, $errfile, $errline) {
        //die("$errstr in $errfile at line $errline: $errstr");
        $this->errorOccured = true;
        $this->assertTrue(false, "$errstr at line $errline, $errfile");
    }
    
    function returnsignore($err)
    {
        $this->wasCalled = true;
        return PEAR_ERRORSTACK_IGNORE;
    }
    
    function test_basic()
    {
        if (!$this->_methodExists('getErrorMessage')) {
            return;
        }
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack,
            array('message' => 'boo', 'params' => array(), 'code' => 6));
        $this->assertEquals('boo', $msg);
    }
    
    function test_basic_template()
    {
        if (!$this->_methodExists('getErrorMessage')) {
            return;
        }
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack,
            array('message' => 'boo', 'params' => array()), 'far%__msg%');
        $this->assertEquals('farboo', $msg);
    }
    
    function test_basic_params()
    {
        if (!$this->_methodExists('getErrorMessage')) {
            return;
        }
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack, array('message' => '',
            'params' => array('bar' => 'hello')), '%bar% foo');
        $this->assertEquals('hello foo', $msg, 'string');
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack, array('message' => '',
            'params' => array('bar' => array('hello', 'there'))), '%bar% foo');
        $this->assertEquals('hello, there foo', $msg, 'array');
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack, array('message' => '',
            'params' => array('bar' => new testgemessage)), '%bar% foo');
        $this->assertEquals('__toString() called foo', $msg, 'first object, __toString()');
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack, array('message' => '',
            'params' => array('bar' => new testgemessage1)), '%bar% foo');
        $this->assertEquals('Object foo', $msg, 'second object, no __toString()');
        $errs = PEAR_ErrorStack::staticGetErrors();
        unset($errs['PEAR_ErrorStack'][0]['context']);
        unset($errs['PEAR_ErrorStack'][0]['time']);
        $this->assertEquals(
            array('PEAR_ErrorStack' =>
            array(
                array(
                    'code' => PEAR_ERRORSTACK_ERR_OBJTOSTRING,
                    'params' => array('obj' => 'testgemessage1'),
                    'package' => 'PEAR_ErrorStack',
                    'level' => 'warning',
                    'message' => 'object testgemessage1 passed into getErrorMessage, but has no __toString() method',
                ))), $errs, 'warning not raised');
    }
    
    function test_basic_params_with_template()
    {
        if (!$this->_methodExists('getErrorMessage')) {
            return;
        }
        $this->stack->setErrorMessageTemplate(array(6 => '%bar% foo'));
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack, array('message' => '',
            'params' => array('bar' => 'hello'), 'code' => 6));
        $this->assertEquals('hello foo', $msg, 'string');
        $msg = PEAR_ErrorStack::getErrorMessage($this->stack, array('message' => '',
            'params' => array('bar' => 'hello'), 'code' => 7));
        $this->assertEquals('', $msg, 'string');
    }
}

?>
