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

class PEAR_ErrorStack_TestCase_singleton extends PHPUnit_TestCase
{
    /**
     * A PEAR_PackageFileManager object
     * @var        object
     */
    var $packagexml;

    function PEAR_ErrorStack_TestCase_singleton($name)
    {
        $this->PHPUnit_TestCase($name);
    }

    function setUp()
    {
        error_reporting(E_ALL);
        $this->errorOccured = false;
        set_error_handler(array(&$this, 'errorHandler'));
        $this->stack = new PEAR_ErrorStack('');
        $s = &PEAR_ErrorStack::singleton('PEAR_ErrorStack');
        $s->pushCallback(array('PEAR_ErrorStack', '_handleError'));
    }

    function tearDown()
    {
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
    
    function test_valid_singleton()
    {
        if (!$this->_methodExists('singleton')) {
            return;
        }
        $one = &PEAR_ErrorStack::singleton('first');
        $two = &PEAR_ErrorStack::singleton('first');
        $two->testme = 2;
        $this->assertEquals(2, $two->testme, 'duh test');
        $one->testme = 4;
        $this->assertEquals(4, $one->testme, 'duh test 2');
        $this->assertEquals(4, $two->testme, 'same object test');
    }
    
    function test_invalid_singleton()
    {
        if (!$this->_methodExists('singleton')) {
            return;
        }
        $one = &PEAR_ErrorStack::singleton('first');
        $two = &PEAR_ErrorStack::singleton('second');
        $two->testme = 2;
        $this->assertEquals(2, $two->testme, 'duh test');
        $one->testme = 4;
        $this->assertEquals(4, $one->testme, 'duh test 2');
        $this->assertEquals(2, $two->testme, 'not same object test');
    }
}

?>
