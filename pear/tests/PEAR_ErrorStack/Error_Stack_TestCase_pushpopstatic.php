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

class PEAR_ErrorStack_TestCase_pushpopstatic extends PHPUnit_TestCase
{
    /**
     * A PEAR_PackageFileManager object
     * @var        object
     */
    var $packagexml;

    function PEAR_ErrorStack_TestCase_pushpopstatic($name)
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
    
    function test_valid_basic()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test');
        PEAR_ErrorStack::staticPush('test', 1);
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
    
    function test_valid_params()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test');
        $z = PEAR_ErrorStack::staticPush('test', 2, 'exception', array('my' => 'param'), 'hello',
            array('test'), array(array('file' => 'boof', 'line' => 34)));
        $err = $this->stack->pop('exception');
        $this->assertEquals($z, $err, 'popped different error');
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 2,
                'params' => array('my' => 'param'),
                'package' => 'test',
                'message' => 'hello',
                'level' => 'exception',
                'context' =>
                    array(
                        'file' => 'boof',
                        'line' => 34,
                    ),
                'repackage' => array('test'),
            ),
            $err, 'popped something else'
        );
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function test_valid_paramscompat()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test', false, null, 'PEAR_ErrorStack', true);
        $z = PEAR_ErrorStack::staticPush('test', 2, 'exception', array('my' => 'param'), 'hello',
            array('test'), array(array('file' => 'boof', 'line' => 34)));
        $this->assertEquals('pear_error', strtolower(get_class($z)), 'not pear_error');
        $err = $this->stack->pop('exception');
        if (is_a($z, 'PEAR_Error')) {
            $this->assertEquals($err, $z->getUserInfo(), 'userinfo wrong');
        }
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 2,
                'params' => array('my' => 'param'),
                'package' => 'test',
                'message' => 'hello',
                'level' => 'exception',
                'context' =>
                    array(
                        'file' => 'boof',
                        'line' => 34,
                    ),
                'repackage' => array('test'),
            ),
            $err, 'popped something else'
        );
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function contextcallback($code, $params, $trace)
    {
        $this->assertEquals(4, $code, 'wrong context code');
        $this->assertEquals(array('hello' => 6), $params, 'wrong context params');
        $this->wasCalled = true;
        return array('hi' => 'there', 'you' => 'fool');
    }
    
    function test_valid_contextsingleton()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test', false, array(&$this, 'contextcallback'));
        $this->wasCalled = false;
        PEAR_ErrorStack::staticPush('test', 4, 'error', array('hello' => 6));
        $this->assertTrue($this->wasCalled, 'context callback was not called!');
        $err = $this->stack->pop();
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 4,
                'params' => array('hello' => 6),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
                'context' => array('hi' => 'there', 'you' => 'fool'),
            ),
            $err, 'popped something else'
        );
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function test_valid_context_setcontext()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        if (!$this->_methodExists('setContextCallback')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test');
        $this->stack->setContextCallback(array(&$this, 'contextcallback'));
        $this->wasCalled = false;
        PEAR_ErrorStack::staticPush('test', 4, 'error', array('hello' => 6));
        $this->assertTrue($this->wasCalled, 'context callback was not called!');
        $err = $this->stack->pop();
        unset($err['time']);
        $this->assertEquals(
            array(
                'code' => 4,
                'params' => array('hello' => 6),
                'package' => 'test',
                'message' => '',
                'level' => 'error',
                'context' => array('hi' => 'there', 'you' => 'fool'),
            ),
            $err, 'popped something else'
        );
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function messagecallback(&$stack, $err)
    {
        $this->assertEquals(4, $err['code'], 'wrong message code');
        $this->assertEquals(array('hello' => 6), $err['params'], 'wrong message params');
        $this->assertEquals('test1', $err['package'], 'wrong error stack');
        $this->wasCalled = true;
        return 'my silly message';
    }
    
    function test_valid_msgcallbacksingleton()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test1', array(&$this, 'messagecallback'));
        $this->wasCalled = false;
        PEAR_ErrorStack::staticPush('test1', 4, 'error', array('hello' => 6));
        $this->assertTrue($this->wasCalled, 'message callback was not called!');
        $err = $this->stack->pop();
        unset($err['time']);
        unset($err['context']);
        $this->assertEquals(
            array(
                'code' => 4,
                'params' => array('hello' => 6),
                'package' => 'test1',
                'message' => 'my silly message',
                'level' => 'error',
            ),
            $err, 'popped something else'
        );
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
    
    function test_valid_msgcallback_setmsgcallback()
    {
        if (!$this->_methodExists('staticPush')) {
            return;
        }
        if (!$this->_methodExists('singleton')) {
            return;
        }
        if (!$this->_methodExists('pop')) {
            return;
        }
        if (!$this->_methodExists('setContextCallback')) {
            return;
        }
        $this->stack = &PEAR_ErrorStack::singleton('test1');
        $this->stack->setMessageCallback(array(&$this, 'messagecallback'));
        $this->wasCalled = false;
        PEAR_ErrorStack::staticPush('test1', 4, 'error', array('hello' => 6));
        $this->assertTrue($this->wasCalled, 'message callback was not called!');
        $err = $this->stack->pop();
        unset($err['time']);
        unset($err['context']);
        $this->assertEquals(
            array(
                'code' => 4,
                'params' => array('hello' => 6),
                'package' => 'test1',
                'message' => 'my silly message',
                'level' => 'error',
            ),
            $err, 'popped something else'
        );
        $err = $this->stack->pop();
        $this->assertFalse($err, 'stack not empty!');
    }
}

?>
