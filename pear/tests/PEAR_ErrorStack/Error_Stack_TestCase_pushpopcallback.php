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

class PEAR_ErrorStack_TestCase_pushpopcallback extends PHPUnit_TestCase
{
    /**
     * A PEAR_PackageFileManager object
     * @var        object
     */
    var $packagexml;

    function PEAR_ErrorStack_TestCase_pushpopcallback($name)
    {
        $this->PHPUnit_TestCase($name);
    }

    function setUp()
    {
        error_reporting(E_ALL);
        $this->errorOccured = false;
        set_error_handler(array(&$this, 'errorHandler'));
        $this->stack = new PEAR_ErrorStack('test');
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
    
    function test_return_ignore()
    {
        if (!$this->_methodExists('push')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        if (!$this->_methodExists('pushCallback')) {
            return;
        }
        if (!$this->_methodExists('popCallback')) {
            return;
        }
        $this->stack->pushCallback(array(&$this, 'returnsignore'));
        $this->wasCalled = false;
        $this->stack->push(1);
        $this->assertTrue($this->wasCalled, 'returnsignore not called');
        $err = $this->stack->pop();
        $this->assertFalse($err, 'error was not ignored!');
        $this->stack->popCallback();
        $this->wasCalled = false;
        $this->stack->push(1);
        $this->assertFalse($this->wasCalled, 'returnsignore called');
        $err = $this->stack->pop();
        unset($err['context']);
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ),
            $err, 'popped something else'
        );
        
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function returnsnothing($err)
    {
        $this->wasCalled = true;
    }
    
    function test_return_nothing()
    {
        if (!$this->_methodExists('push')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        if (!$this->_methodExists('pushCallback')) {
            return;
        }
        if (!$this->_methodExists('popCallback')) {
            return;
        }
        $this->stack->pushCallback(array(&$this, 'returnsnothing'));
        $this->wasCalled = false;
        $this->stack->push(1);
        $this->assertTrue($this->wasCalled, 'returnsnothing not called');
        $err = $this->stack->pop();
        unset($err['context']);
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ),
            $err, 'popped something else'
        );
        $this->stack->popCallback();
        $this->wasCalled = false;
        $this->stack->push(1);
        $this->assertFalse($this->wasCalled, 'returnsnothing called');
        $err = $this->stack->pop();
        unset($err['context']);
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ),
            $err, 'popped something else'
        );
        
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function returnspush($err)
    {
        $this->wasCalled = true;
        return PEAR_ERRORSTACK_PUSH;
    }
    
    function test_return_push()
    {
        if (!$this->_methodExists('push')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        if (!$this->_methodExists('pushCallback')) {
            return;
        }
        if (!$this->_methodExists('popCallback')) {
            return;
        }
        if (!$this->_methodExists('setLogger')) {
            return;
        }
        $this->stack->pushCallback(array(&$this, 'returnspush'));
        $log = new BurfLog;
        $log->setTestCase($this);
        $log->curMethod(__FUNCTION__);
        $this->stack->setLogger($log);
        $this->wasCalled = false;
        $this->stack->push(1);
        $this->assertTrue($this->wasCalled, 'returnspush not called');
        $err = $this->stack->pop();
        unset($err['context']);
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ),
            $err, 'popped something else 1'
        );
        $this->stack->popCallback();
        $log->pushExpect('', PEAR_LOG_ERR, array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ));
        $this->wasCalled = false;
        $this->wasLogged = false;
        $this->stack->push(1);
        $this->assertFalse($this->wasCalled, 'returnspush called');
        $this->assertTrue($this->wasLogged, 'was not logged!');
        $err = $this->stack->pop();
        unset($err['context']);
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ),
            $err, 'popped something else 2'
        );
        
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function returnslog($err)
    {
        $this->wasCalled = true;
        return PEAR_ERRORSTACK_LOG;
    }
    
    function test_return_log()
    {
        if (!$this->_methodExists('push')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        if (!$this->_methodExists('pushCallback')) {
            return;
        }
        if (!$this->_methodExists('popCallback')) {
            return;
        }
        if (!$this->_methodExists('setLogger')) {
            return;
        }
        $this->stack->pushCallback(array(&$this, 'returnslog'));
        $log = new BurfLog;
        $log->setTestCase($this);
        $log->curMethod(__FUNCTION__);
        $this->stack->setLogger($log);
        $this->wasCalled = false;
        $this->wasLogged = false;
        $log->pushExpect('', PEAR_LOG_ERR, array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ));
        $this->stack->push(1);
        $this->assertTrue($this->wasCalled, 'returnslog not called');
        $this->assertTrue($this->wasLogged, 'was not logged!');
        $err = $this->stack->pop();
        $this->assertFalse($err, 'an error was pushed!');
        $this->stack->popCallback();
        $log->clearExpect();
        $log->pushExpect('', PEAR_LOG_ERR, array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ));
        $this->wasCalled = false;
        $this->wasLogged = false;
        $this->stack->push(1);
        $this->assertFalse($this->wasCalled, 'returnspush called');
        $this->assertTrue($this->wasLogged, 'was not logged!');
        $err = $this->stack->pop();
        unset($err['context']);
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 1,
                'params' => array(),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
            ),
            $err, 'popped something else 2'
        );
        
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
}

?>
