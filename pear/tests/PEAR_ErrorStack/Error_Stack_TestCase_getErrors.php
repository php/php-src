<?php

/**
 * API Unit tests for PEAR_ErrorStack package.
 * 
 * @version    $Id$
 * @author     Laurent Laville <pear@laurent-laville.org> portions from HTML_CSS
 * @author     Greg Beaver
 * @package    PEAR_ErrorStack
 */

/**
 * @package PEAR_ErrorStack
 */

class PEAR_ErrorStack_TestCase_getErrors extends PHPUnit_TestCase
{

    function PEAR_ErrorStack_TestCase_getErrors($name)
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
    
    function test_none()
    {
        if (!$this->_methodExists('getErrors')) {
            return;
        }
        $this->assertEquals(array(), $this->stack->getErrors());
        $this->assertEquals(array(), $this->stack->getErrors(true));
    }
    
    function test_normal()
    {
        if (!$this->_methodExists('getErrors')) {
            return;
        }
        $this->assertEquals(array(), $this->stack->getErrors());
        $this->stack->push(1);
        $this->stack->push(2, 'warning');
        $this->stack->push(3, 'foo');
        $ret = $this->stack->getErrors();
        for ($i= 0; $i < 3; $i++) {
            unset($ret[$i]['time']);
            unset($ret[$i]['context']);
        }
        $this->assertEquals(
            array(
                array('code' => 3,
                'params' => array(),
                'package' => 'test',
                'level' => 'foo',
                'message' => ''),
                array('code' => 2,
                'params' => array(),
                'package' => 'test',
                'level' => 'warning',
                'message' => ''),
                array('code' => 1,
                'params' => array(),
                'package' => 'test',
                'level' => 'error',
                'message' => ''),
                ), $ret, 'incorrect errors, non-purge');
        $ret = $this->stack->getErrors(true);
        for ($i= 0; $i < 3; $i++) {
            unset($ret[$i]['time']);
            unset($ret[$i]['context']);
        }
        $this->assertEquals(
            array(
                array('code' => 3,
                'params' => array(),
                'package' => 'test',
                'level' => 'foo',
                'message' => ''),
                array('code' => 2,
                'params' => array(),
                'package' => 'test',
                'level' => 'warning',
                'message' => ''),
                array('code' => 1,
                'params' => array(),
                'package' => 'test',
                'level' => 'error',
                'message' => ''),
                ), $ret, 'incorrect errors, purge');
        $this->assertEquals(array(), $this->stack->getErrors());
    }
}

?>
