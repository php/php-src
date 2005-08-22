<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Gregory Beaver <cellog@php.net>                              |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

/**
 * Error Stack Implementation
 * 
 * This is an incredibly simple implementation of a very complex error handling
 * facility.  It contains the ability
 * to track multiple errors from multiple packages simultaneously.  In addition,
 * it can track errors of many levels, save data along with the error, context
 * information such as the exact file, line number, class and function that
 * generated the error, and if necessary, it can raise a traditional PEAR_Error.
 * It has built-in support for PEAR::Log, to log errors as they occur
 * 
 * Since version 0.2alpha, it is also possible to selectively ignore errors,
 * through the use of an error callback, see {@link pushCallback()}
 * 
 * Since version 0.3alpha, it is possible to specify the exception class
 * returned from {@link push()}
 *
 * Since version PEAR1.3.2, ErrorStack no longer instantiates an exception class.  This can
 * still be done quite handily in an error callback or by manipulating the returned array
 * @author Greg Beaver <cellog@php.net>
 * @version PEAR1.3.2 (beta)
 * @package PEAR_ErrorStack
 * @category Debugging
 * @license http://www.php.net/license/3_0.txt PHP License v3.0
 */

/**
 * Singleton storage
 * 
 * Format:
 * <pre>
 * array(
 *  'package1' => PEAR_ErrorStack object,
 *  'package2' => PEAR_ErrorStack object,
 *  ...
 * )
 * </pre>
 * @access private
 * @global array $GLOBALS['_PEAR_ERRORSTACK_SINGLETON']
 */
$GLOBALS['_PEAR_ERRORSTACK_SINGLETON'] = array();

/**
 * Global error callback (default)
 * 
 * This is only used if set to non-false.  * is the default callback for
 * all packages, whereas specific packages may set a default callback
 * for all instances, regardless of whether they are a singleton or not.
 *
 * To exclude non-singletons, only set the local callback for the singleton
 * @see PEAR_ErrorStack::setDefaultCallback()
 * @access private
 * @global array $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_CALLBACK']
 */
$GLOBALS['_PEAR_ERRORSTACK_DEFAULT_CALLBACK'] = array(
    '*' => false,
);

/**
 * Global Log object (default)
 * 
 * This is only used if set to non-false.  Use to set a default log object for
 * all stacks, regardless of instantiation order or location
 * @see PEAR_ErrorStack::setDefaultLogger()
 * @access private
 * @global array $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER']
 */
$GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER'] = false;

/**
 * Global Overriding Callback
 * 
 * This callback will override any error callbacks that specific loggers have set.
 * Use with EXTREME caution
 * @see PEAR_ErrorStack::staticPushCallback()
 * @access private
 * @global array $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER']
 */
$GLOBALS['_PEAR_ERRORSTACK_OVERRIDE_CALLBACK'] = array();

/**#@+
 * One of four possible return values from the error Callback
 * @see PEAR_ErrorStack::_errorCallback()
 */
/**
 * If this is returned, then the error will be both pushed onto the stack
 * and logged.
 */
define('PEAR_ERRORSTACK_PUSHANDLOG', 1);
/**
 * If this is returned, then the error will only be pushed onto the stack,
 * and not logged.
 */
define('PEAR_ERRORSTACK_PUSH', 2);
/**
 * If this is returned, then the error will only be logged, but not pushed
 * onto the error stack.
 */
define('PEAR_ERRORSTACK_LOG', 3);
/**
 * If this is returned, then the error is completely ignored.
 */
define('PEAR_ERRORSTACK_IGNORE', 4);
/**
 * If this is returned, then the error is logged and die() is called.
 */
define('PEAR_ERRORSTACK_DIE', 5);
/**#@-*/

/**
 * Error code for an attempt to instantiate a non-class as a PEAR_ErrorStack in
 * the singleton method.
 */
define('PEAR_ERRORSTACK_ERR_NONCLASS', 1);

/**
 * Error code for an attempt to pass an object into {@link PEAR_ErrorStack::getMessage()}
 * that has no __toString() method
 */
define('PEAR_ERRORSTACK_ERR_OBJTOSTRING', 2);
/**
 * Error Stack Implementation
 *
 * Usage:
 * <code>
 * // global error stack
 * $global_stack = &PEAR_ErrorStack::singleton('MyPackage');
 * // local error stack
 * $local_stack = new PEAR_ErrorStack('MyPackage');
 * </code>
 * @copyright 2004 Gregory Beaver
 * @package PEAR_ErrorStack
 * @license http://www.php.net/license/3_0.txt PHP License
 */
class PEAR_ErrorStack {
    /**
     * Errors are stored in the order that they are pushed on the stack.
     * @since 0.4alpha Errors are no longer organized by error level.
     * This renders pop() nearly unusable, and levels could be more easily
     * handled in a callback anyway
     * @var array
     * @access private
     */
    var $_errors = array();

    /**
     * Storage of errors by level.
     *
     * Allows easy retrieval and deletion of only errors from a particular level
     * @since PEAR 1.4.0dev
     * @var array
     * @access private
     */
    var $_errorsByLevel = array();

    /**
     * Package name this error stack represents
     * @var string
     * @access protected
     */
    var $_package;
    
    /**
     * Determines whether a PEAR_Error is thrown upon every error addition
     * @var boolean
     * @access private
     */
    var $_compat = false;
    
    /**
     * If set to a valid callback, this will be used to generate the error
     * message from the error code, otherwise the message passed in will be
     * used
     * @var false|string|array
     * @access private
     */
    var $_msgCallback = false;
    
    /**
     * If set to a valid callback, this will be used to generate the error
     * context for an error.  For PHP-related errors, this will be a file
     * and line number as retrieved from debug_backtrace(), but can be
     * customized for other purposes.  The error might actually be in a separate
     * configuration file, or in a database query.
     * @var false|string|array
     * @access protected
     */
    var $_contextCallback = false;
    
    /**
     * If set to a valid callback, this will be called every time an error
     * is pushed onto the stack.  The return value will be used to determine
     * whether to allow an error to be pushed or logged.
     * 
     * The return value must be one an PEAR_ERRORSTACK_* constant
     * @see PEAR_ERRORSTACK_PUSHANDLOG, PEAR_ERRORSTACK_PUSH, PEAR_ERRORSTACK_LOG
     * @var false|string|array
     * @access protected
     */
    var $_errorCallback = array();
    
    /**
     * PEAR::Log object for logging errors
     * @var false|Log
     * @access protected
     */
    var $_logger = false;
    
    /**
     * Error messages - designed to be overridden
     * @var array
     * @abstract
     */
    var $_errorMsgs = array();
    
    /**
     * Set up a new error stack
     * 
     * @param string   $package name of the package this error stack represents
     * @param callback $msgCallback callback used for error message generation
     * @param callback $contextCallback callback used for context generation,
     *                 defaults to {@link getFileLine()}
     * @param boolean  $throwPEAR_Error
     */
    function PEAR_ErrorStack($package, $msgCallback = false, $contextCallback = false,
                         $throwPEAR_Error = false)
    {
        $this->_package = $package;
        $this->setMessageCallback($msgCallback);
        $this->setContextCallback($contextCallback);
        $this->_compat = $throwPEAR_Error;
    }
    
    /**
     * Return a single error stack for this package.
     * 
     * Note that all parameters are ignored if the stack for package $package
     * has already been instantiated
     * @param string   $package name of the package this error stack represents
     * @param callback $msgCallback callback used for error message generation
     * @param callback $contextCallback callback used for context generation,
     *                 defaults to {@link getFileLine()}
     * @param boolean  $throwPEAR_Error
     * @param string   $stackClass class to instantiate
     * @static
     * @return PEAR_ErrorStack
     */
    function &singleton($package, $msgCallback = false, $contextCallback = false,
                         $throwPEAR_Error = false, $stackClass = 'PEAR_ErrorStack')
    {
        if (isset($GLOBALS['_PEAR_ERRORSTACK_SINGLETON'][$package])) {
            return $GLOBALS['_PEAR_ERRORSTACK_SINGLETON'][$package];
        }
        if (!class_exists($stackClass)) {
            if (function_exists('debug_backtrace')) {
                $trace = debug_backtrace();
            }
            PEAR_ErrorStack::staticPush('PEAR_ErrorStack', PEAR_ERRORSTACK_ERR_NONCLASS,
                'exception', array('stackclass' => $stackClass),
                'stack class "%stackclass%" is not a valid class name (should be like PEAR_ErrorStack)',
                false, $trace);
        }
        return $GLOBALS['_PEAR_ERRORSTACK_SINGLETON'][$package] =
            &new $stackClass($package, $msgCallback, $contextCallback, $throwPEAR_Error);
    }

    /**
     * Internal error handler for PEAR_ErrorStack class
     * 
     * Dies if the error is an exception (and would have died anyway)
     * @access private
     */
    function _handleError($err)
    {
        if ($err['level'] == 'exception') {
            $message = $err['message'];
            if (isset($_SERVER['REQUEST_URI'])) {
                echo '<br />';
            } else {
                echo "\n";
            }
            var_dump($err['context']);
            die($message);
        }
    }
    
    /**
     * Set up a PEAR::Log object for all error stacks that don't have one
     * @param Log $log 
     * @static
     */
    function setDefaultLogger(&$log)
    {
        if (is_object($log) && method_exists($log, 'log') ) {
            $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER'] = &$log;
        } elseif (is_callable($log)) {
            $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER'] = &$log;
	}
    }
    
    /**
     * Set up a PEAR::Log object for this error stack
     * @param Log $log 
     */
    function setLogger(&$log)
    {
        if (is_object($log) && method_exists($log, 'log') ) {
            $this->_logger = &$log;
        } elseif (is_callable($log)) {
            $this->_logger = &$log;
        }
    }
    
    /**
     * Set an error code => error message mapping callback
     * 
     * This method sets the callback that can be used to generate error
     * messages for any instance
     * @param array|string Callback function/method
     */
    function setMessageCallback($msgCallback)
    {
        if (!$msgCallback) {
            $this->_msgCallback = array(&$this, 'getErrorMessage');
        } else {
            if (is_callable($msgCallback)) {
                $this->_msgCallback = $msgCallback;
            }
        }
    }
    
    /**
     * Get an error code => error message mapping callback
     * 
     * This method returns the current callback that can be used to generate error
     * messages
     * @return array|string|false Callback function/method or false if none
     */
    function getMessageCallback()
    {
        return $this->_msgCallback;
    }
    
    /**
     * Sets a default callback to be used by all error stacks
     * 
     * This method sets the callback that can be used to generate error
     * messages for a singleton
     * @param array|string Callback function/method
     * @param string Package name, or false for all packages
     * @static
     */
    function setDefaultCallback($callback = false, $package = false)
    {
        if (!is_callable($callback)) {
            $callback = false;
        }
        $package = $package ? $package : '*';
        $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_CALLBACK'][$package] = $callback;
    }
    
    /**
     * Set a callback that generates context information (location of error) for an error stack
     * 
     * This method sets the callback that can be used to generate context
     * information for an error.  Passing in NULL will disable context generation
     * and remove the expensive call to debug_backtrace()
     * @param array|string|null Callback function/method
     */
    function setContextCallback($contextCallback)
    {
        if ($contextCallback === null) {
            return $this->_contextCallback = false;
        }
        if (!$contextCallback) {
            $this->_contextCallback = array(&$this, 'getFileLine');
        } else {
            if (is_callable($contextCallback)) {
                $this->_contextCallback = $contextCallback;
            }
        }
    }
    
    /**
     * Set an error Callback
     * If set to a valid callback, this will be called every time an error
     * is pushed onto the stack.  The return value will be used to determine
     * whether to allow an error to be pushed or logged.
     * 
     * The return value must be one of the ERRORSTACK_* constants.
     * 
     * This functionality can be used to emulate PEAR's pushErrorHandling, and
     * the PEAR_ERROR_CALLBACK mode, without affecting the integrity of
     * the error stack or logging
     * @see PEAR_ERRORSTACK_PUSHANDLOG, PEAR_ERRORSTACK_PUSH, PEAR_ERRORSTACK_LOG
     * @see popCallback()
     * @param string|array $cb
     */
    function pushCallback($cb)
    {
        array_push($this->_errorCallback, $cb);
    }
    
    /**
     * Remove a callback from the error callback stack
     * @see pushCallback()
     * @return array|string|false
     */
    function popCallback()
    {
        if (!count($this->_errorCallback)) {
            return false;
        }
        return array_pop($this->_errorCallback);
    }
    
    /**
     * Set a temporary overriding error callback for every package error stack
     *
     * Use this to temporarily disable all existing callbacks (can be used
     * to emulate the @ operator, for instance)
     * @see PEAR_ERRORSTACK_PUSHANDLOG, PEAR_ERRORSTACK_PUSH, PEAR_ERRORSTACK_LOG
     * @see staticPopCallback(), pushCallback()
     * @param string|array $cb
     * @static
     */
    function staticPushCallback($cb)
    {
        array_push($GLOBALS['_PEAR_ERRORSTACK_OVERRIDE_CALLBACK'], $cb);
    }
    
    /**
     * Remove a temporary overriding error callback
     * @see staticPushCallback()
     * @return array|string|false
     * @static
     */
    function staticPopCallback()
    {
        $ret = array_pop($GLOBALS['_PEAR_ERRORSTACK_OVERRIDE_CALLBACK']);
        if (!is_array($GLOBALS['_PEAR_ERRORSTACK_OVERRIDE_CALLBACK'])) {
            $GLOBALS['_PEAR_ERRORSTACK_OVERRIDE_CALLBACK'] = array();
        }
        return $ret;
    }
    
    /**
     * Add an error to the stack
     * 
     * If the message generator exists, it is called with 2 parameters.
     *  - the current Error Stack object
     *  - an array that is in the same format as an error.  Available indices
     *    are 'code', 'package', 'time', 'params', 'level', and 'context'
     * 
     * Next, if the error should contain context information, this is
     * handled by the context grabbing method.
     * Finally, the error is pushed onto the proper error stack
     * @param int    $code      Package-specific error code
     * @param string $level     Error level.  This is NOT spell-checked
     * @param array  $params    associative array of error parameters
     * @param string $msg       Error message, or a portion of it if the message
     *                          is to be generated
     * @param array  $repackage If this error re-packages an error pushed by
     *                          another package, place the array returned from
     *                          {@link pop()} in this parameter
     * @param array  $backtrace Protected parameter: use this to pass in the
     *                          {@link debug_backtrace()} that should be used
     *                          to find error context
     * @return PEAR_Error|array|Exception
     *                          if compatibility mode is on, a PEAR_Error is also
     *                          thrown.  If the class Exception exists, then one
     *                          is returned to allow code like:
     * <code>
     * throw ($stack->push(MY_ERROR_CODE, 'error', array('username' => 'grob')));
     * </code>
     * 
     * The errorData property of the exception class will be set to the array
     * that would normally be returned.  If a PEAR_Error is returned, the userinfo
     * property is set to the array
     * 
     * Otherwise, an array is returned in this format:
     * <code>
     * array(
     *    'code' => $code,
     *    'params' => $params,
     *    'package' => $this->_package,
     *    'level' => $level,
     *    'time' => time(),
     *    'context' => $context,
     *    'message' => $msg,
     * //['repackage' => $err] repackaged error array/Exception class
     * );
     * </code>
     */
    function push($code, $level = 'error', $params = array(), $msg = false,
                  $repackage = false, $backtrace = false)
    {
        $context = false;
        // grab error context
        if ($this->_contextCallback) {
            if (!$backtrace) {
                $backtrace = debug_backtrace();
            }
            $context = call_user_func($this->_contextCallback, $code, $params, $backtrace);
        }
        
        // save error
        $time = explode(' ', microtime());
        $time = $time[1] + $time[0];
        $err = array(
                'code' => $code,
                'params' => $params,
                'package' => $this->_package,
                'level' => $level,
                'time' => $time,
                'context' => $context,
                'message' => $msg,
               );

        if ($repackage) {
            $err['repackage'] = $repackage;
        }

        // set up the error message, if necessary
        if ($this->_msgCallback) {
            $msg = call_user_func_array($this->_msgCallback,
                                        array(&$this, $err));
            $err['message'] = $msg;
        }        
        
        $push = $log = true;
        $die = false;
        // try the overriding callback first
        $callback = $this->staticPopCallback();
        if ($callback) {
            $this->staticPushCallback($callback);
        }
        if (!is_callable($callback)) {
            // try the local callback next
            $callback = $this->popCallback();
            if (is_callable($callback)) {
                $this->pushCallback($callback);
            } else {
                // try the default callback
                $callback = isset($GLOBALS['_PEAR_ERRORSTACK_DEFAULT_CALLBACK'][$this->_package]) ?
                    $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_CALLBACK'][$this->_package] :
                    $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_CALLBACK']['*'];
            }
        }
        if (is_callable($callback)) {
            switch(call_user_func($callback, $err)){
            	case PEAR_ERRORSTACK_IGNORE: 
            		return $err;
        		break;
            	case PEAR_ERRORSTACK_PUSH: 
            		$log = false;
        		break;
            	case PEAR_ERRORSTACK_LOG: 
            		$push = false;
        		break;
            	case PEAR_ERRORSTACK_DIE: 
            		$die = true;
        		break;
                // anything else returned has the same effect as pushandlog
            }
        }
        if ($push) {
            array_unshift($this->_errors, $err);
            $this->_errorsByLevel[$err['level']][] = &$this->_errors[0];
        }
        if ($log) {
            if ($this->_logger || $GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER']) {
                $this->_log($err);
            }
        }
        if ($die) {
            die();
        }
        if ($this->_compat && $push) {
            return $this->raiseError($msg, $code, null, null, $err);
        }
        return $err;
    }
    
    /**
     * Static version of {@link push()}
     * 
     * @param string $package   Package name this error belongs to
     * @param int    $code      Package-specific error code
     * @param string $level     Error level.  This is NOT spell-checked
     * @param array  $params    associative array of error parameters
     * @param string $msg       Error message, or a portion of it if the message
     *                          is to be generated
     * @param array  $repackage If this error re-packages an error pushed by
     *                          another package, place the array returned from
     *                          {@link pop()} in this parameter
     * @param array  $backtrace Protected parameter: use this to pass in the
     *                          {@link debug_backtrace()} that should be used
     *                          to find error context
     * @return PEAR_Error|null|Exception
     *                          if compatibility mode is on, a PEAR_Error is also
     *                          thrown.  If the class Exception exists, then one
     *                          is returned to allow code like:
     * <code>
     * throw ($stack->push(MY_ERROR_CODE, 'error', array('username' => 'grob')));
     * </code>
     * @static
     */
    function staticPush($package, $code, $level = 'error', $params = array(),
                        $msg = false, $repackage = false, $backtrace = false)
    {
        $s = &PEAR_ErrorStack::singleton($package);
        if ($s->_contextCallback) {
            if (!$backtrace) {
                if (function_exists('debug_backtrace')) {
                    $backtrace = debug_backtrace();
                }
            }
        }
        return $s->push($code, $level, $params, $msg, $repackage, $backtrace);
    }
    
    /**
     * Log an error using PEAR::Log
     * @param array $err Error array
     * @param array $levels Error level => Log constant map
     * @access protected
     */
    function _log($err)
    {
        if ($this->_logger) {
            $logger = &$this->_logger;
        } else {
            $logger = &$GLOBALS['_PEAR_ERRORSTACK_DEFAULT_LOGGER'];
        }
        if (is_a($logger, 'Log')) {
            $levels = array(
                'exception' => PEAR_LOG_CRIT,
                'alert' => PEAR_LOG_ALERT,
                'critical' => PEAR_LOG_CRIT,
                'error' => PEAR_LOG_ERR,
                'warning' => PEAR_LOG_WARNING,
                'notice' => PEAR_LOG_NOTICE,
                'info' => PEAR_LOG_INFO,
                'debug' => PEAR_LOG_DEBUG);
            if (isset($levels[$err['level']])) {
                $level = $levels[$err['level']];
            } else {
                $level = PEAR_LOG_INFO;
            }
            $logger->log($err['message'], $level, $err);
        } else { // support non-standard logs
            call_user_func($logger, $err);
        }
    }

    
    /**
     * Pop an error off of the error stack
     * 
     * @return false|array
     * @since 0.4alpha it is no longer possible to specify a specific error
     * level to return - the last error pushed will be returned, instead
     */
    function pop()
    {
        return @array_shift($this->_errors);
    }
    
    /**
     * Determine whether there are any errors on the stack
     * @param string|array Level name.  Use to determine if any errors
     * of level (string), or levels (array) have been pushed
     * @return boolean
     */
    function hasErrors($level = false)
    {
        if ($level) {
            return isset($this->_errorsByLevel[$level]);
        }
        return count($this->_errors);
    }
    
    /**
     * Retrieve all errors since last purge
     * 
     * @param boolean set in order to empty the error stack
     * @param string level name, to return only errors of a particular severity
     * @return array
     */
    function getErrors($purge = false, $level = false)
    {
        if (!$purge) {
            if ($level) {
                if (!isset($this->_errorsByLevel[$level])) {
                    return array();
                } else {
                    return $this->_errorsByLevel[$level];
                }
            } else {
                return $this->_errors;
            }
        }
        if ($level) {
            $ret = $this->_errorsByLevel[$level];
            foreach ($this->_errorsByLevel[$level] as $i => $unused) {
                // entries are references to the $_errors array
                $this->_errorsByLevel[$level][$i] = false;
            }
            // array_filter removes all entries === false
            $this->_errors = array_filter($this->_errors);
            unset($this->_errorsByLevel[$level]);
            return $ret;
        }
        $ret = $this->_errors;
        $this->_errors = array();
        $this->_errorsByLevel = array();
        return $ret;
    }
    
    /**
     * Determine whether there are any errors on a single error stack, or on any error stack
     *
     * The optional parameter can be used to test the existence of any errors without the need of
     * singleton instantiation
     * @param string|false Package name to check for errors
     * @param string Level name to check for a particular severity
     * @return boolean
     * @static
     */
    function staticHasErrors($package = false, $level = false)
    {
        if ($package) {
            if (!isset($GLOBALS['_PEAR_ERRORSTACK_SINGLETON'][$package])) {
                return false;
            }
            return $GLOBALS['_PEAR_ERRORSTACK_SINGLETON'][$package]->hasErrors($level);
        }
        foreach ($GLOBALS['_PEAR_ERRORSTACK_SINGLETON'] as $package => $obj) {
            if ($obj->hasErrors($level)) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Get a list of all errors since last purge, organized by package
     * @since PEAR 1.4.0dev BC break! $level is now in the place $merge used to be
     * @param boolean $purge Set to purge the error stack of existing errors
     * @param string  $level Set to a level name in order to retrieve only errors of a particular level
     * @param boolean $merge Set to return a flat array, not organized by package
     * @param array   $sortfunc Function used to sort a merged array - default
     *        sorts by time, and should be good for most cases
     * @static
     * @return array 
     */
    function staticGetErrors($purge = false, $level = false, $merge = false,
                             $sortfunc = array('PEAR_ErrorStack', '_sortErrors'))
    {
        $ret = array();
        if (!is_callable($sortfunc)) {
            $sortfunc = array('PEAR_ErrorStack', '_sortErrors');
        }
        foreach ($GLOBALS['_PEAR_ERRORSTACK_SINGLETON'] as $package => $obj) {
            $test = $GLOBALS['_PEAR_ERRORSTACK_SINGLETON'][$package]->getErrors($purge, $level);
            if ($test) {
                if ($merge) {
                    $ret = array_merge($ret, $test);
                } else {
                    $ret[$package] = $test;
                }
            }
        }
        if ($merge) {
            usort($ret, $sortfunc);
        }
        return $ret;
    }
    
    /**
     * Error sorting function, sorts by time
     * @access private
     */
    function _sortErrors($a, $b)
    {
        if ($a['time'] == $b['time']) {
            return 0;
        }
        if ($a['time'] < $b['time']) {
            return 1;
        }
        return -1;
    }

    /**
     * Standard file/line number/function/class context callback
     *
     * This function uses a backtrace generated from {@link debug_backtrace()}
     * and so will not work at all in PHP < 4.3.0.  The frame should
     * reference the frame that contains the source of the error.
     * @return array|false either array('file' => file, 'line' => line,
     *         'function' => function name, 'class' => class name) or
     *         if this doesn't work, then false
     * @param unused
     * @param integer backtrace frame.
     * @param array Results of debug_backtrace()
     * @static
     */
    function getFileLine($code, $params, $backtrace = null)
    {
        if ($backtrace === null) {
            return false;
        }
        $frame = 0;
        $functionframe = 1;
        if (!isset($backtrace[1])) {
            $functionframe = 0;
        } else {
            while (isset($backtrace[$functionframe]['function']) &&
                  $backtrace[$functionframe]['function'] == 'eval' &&
                  isset($backtrace[$functionframe + 1])) {
                $functionframe++;
            }
        }
        if (isset($backtrace[$frame])) {
            if (!isset($backtrace[$frame]['file'])) {
                $frame++;
            }
            $funcbacktrace = $backtrace[$functionframe];
            $filebacktrace = $backtrace[$frame];
            $ret = array('file' => $filebacktrace['file'],
                         'line' => $filebacktrace['line']);
            // rearrange for eval'd code or create function errors
            if (strpos($filebacktrace['file'], '(') && 
            	  preg_match(';^(.*?)\((\d+)\) : (.*?)$;', $filebacktrace['file'],
                  $matches)) {
                $ret['file'] = $matches[1];
                $ret['line'] = $matches[2] + 0;
            }
            if (isset($funcbacktrace['function']) && isset($backtrace[1])) {
                if ($funcbacktrace['function'] != 'eval') {
                    if ($funcbacktrace['function'] == '__lambda_func') {
                        $ret['function'] = 'create_function() code';
                    } else {
                        $ret['function'] = $funcbacktrace['function'];
                    }
                }
            }
            if (isset($funcbacktrace['class']) && isset($backtrace[1])) {
                $ret['class'] = $funcbacktrace['class'];
            }
            return $ret;
        }
        return false;
    }
    
    /**
     * Standard error message generation callback
     * 
     * This method may also be called by a custom error message generator
     * to fill in template values from the params array, simply
     * set the third parameter to the error message template string to use
     * 
     * The special variable %__msg% is reserved: use it only to specify
     * where a message passed in by the user should be placed in the template,
     * like so:
     * 
     * Error message: %msg% - internal error
     * 
     * If the message passed like so:
     * 
     * <code>
     * $stack->push(ERROR_CODE, 'error', array(), 'server error 500');
     * </code>
     * 
     * The returned error message will be "Error message: server error 500 -
     * internal error"
     * @param PEAR_ErrorStack
     * @param array
     * @param string|false Pre-generated error message template
     * @static
     * @return string
     */
    function getErrorMessage(&$stack, $err, $template = false)
    {
        if ($template) {
            $mainmsg = $template;
        } else {
            $mainmsg = $stack->getErrorMessageTemplate($err['code']);
        }

        $mainmsg = str_replace('%__msg%', $err['message'], $mainmsg);

        if (count($err['params']) && count($err['params'])) {
            foreach ($err['params'] as $name => $val) {
                if (is_array($val)) {
                    // @ is needed in case $val is a multi-dimensional array
                    $val = @implode(', ', $val);
                }
                if (is_object($val)) {
                    if (method_exists($val, '__toString')) {
                        $val = $val->__toString();
                    } else {
                        PEAR_ErrorStack::staticPush('PEAR_ErrorStack', PEAR_ERRORSTACK_ERR_OBJTOSTRING,
                            'warning', array('obj' => get_class($val)),
                            'object %obj% passed into getErrorMessage, but has no __toString() method');
                        $val = 'Object';
                    }
                }
                $mainmsg = str_replace('%' . $name . '%', $val, $mainmsg);
            }
        }
        return $mainmsg;
    }
    
    /**
     * Standard Error Message Template generator from code
     * @return string
     */
    function getErrorMessageTemplate($code)
    {
        if (!isset($this->_errorMsgs[$code])) {
            return '%__msg%';
        }
        return $this->_errorMsgs[$code];
    }
    
    /**
     * Set the Error Message Template array
     * 
     * The array format must be:
     * <pre>
     * array(error code => 'message template',...)
     * </pre>
     * 
     * Error message parameters passed into {@link push()} will be used as input
     * for the error message.  If the template is 'message %foo% was %bar%', and the
     * parameters are array('foo' => 'one', 'bar' => 'six'), the error message returned will
     * be 'message one was six'
     * @return string
     */
    function setErrorMessageTemplate($template)
    {
        $this->_errorMsgs = $template;
    }
    
    
    /**
     * emulate PEAR::raiseError()
     * 
     * @return PEAR_Error
     */
    function raiseError()
    {
        require_once 'PEAR.php';
        $args = func_get_args();
        return call_user_func_array(array('PEAR', 'raiseError'), $args);
    }
}
$stack = &PEAR_ErrorStack::singleton('PEAR_ErrorStack');
$stack->pushCallback(array('PEAR_ErrorStack', '_handleError'));
?>
