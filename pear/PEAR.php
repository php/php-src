<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Sterling Hughes <sterling@php.net>                          |
// |          Stig Bakken <ssb@fast.no>                                   |
// +----------------------------------------------------------------------+
//
// $Id$
//

define('PEAR_ERROR_RETURN', 0);
define('PEAR_ERROR_PRINT', 1);
define('PEAR_ERROR_TRIGGER', 2);
define('PEAR_ERROR_DIE', 3);

$_PEAR_destructor_object_list = array();

/**
 * Base class for other PEAR classes.  Provides rudimentary
 * emulation of destructors.
 *
 * If you want a destructor in your class, inherit PEAR and make a
 * destructor method called _yourclassname (same name as the
 * constructor, but with a "_" prefix).  Also, in your constructor you
 * have to call the PEAR constructor: "$this->PEAR();".  The
 * destructor method will be called without parameters.  Note that at
 * in some SAPI implementations (such as Apache), any output during
 * the request shutdown (in which destructors are called) seems to be
 * discarded.  If you need to get any debug information from your
 * destructor, use error_log(), syslog() or something like that
 * instead.
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR {
    // {{{ constructor

    /**
     * Constructor.  Registers this object in
     * $_PEAR_destructor_object_list for destructor emulation.
     */
    function PEAR() {
	global $_PEAR_destructor_object_list;
	$_PEAR_destructor_object_list[] = &$this;
    }

    // }}}
    // {{{ destructor

    /**
     * Destructor (the emulated type of...).
     * 
     * See the note in the class desciption about output from
     * destructors.
     */
    function _PEAR() {
    }

    // }}}
    // {{{ isError()

    /**
     * Tell whether a value is a PEAR error.
     *
     * @param $data the value to test
     *
     * @return bool true if $data is an error
     */
    function isError(&$data) {
	return is_object($data) && is_subclass_of($data, "PEAR_Error");
    }

    // }}}
}

// {{{ _PEAR_call_destructors()

function _PEAR_call_destructors() {
    global $_PEAR_destructor_object_list;
    if (is_array($_PEAR_destructor_object_list) && sizeof($_PEAR_destructor_object_list)) {
	reset($_PEAR_destructor_object_list);
	while (list($k, $objref) = each($_PEAR_destructor_object_list)) {
	    $destructor = "_".get_class($objref);
	    if (method_exists($objref, $destructor)) {
		$objref->$destructor();
	    }
	}
	// Empty the object list to ensure that destructors are
	// not called more than once.
	$_PEAR_destructor_object_list = array();
    }
}

// }}}

class PEAR_Error
{
    // {{{ properties

	var $classname            = '';
	var $error_message_prefix = '';
	var $error_prepend        = '';
	var $error_append         = '';
	
	var $mode                 = PEAR_ERROR_RETURN;
	var $level                = E_USER_NOTICE;
	
	var $message              = '';

	// Wait until we have a stack-groping function in PHP.
	//var $file    = '';
	//var $line    = 0;
	

    // }}}
    // {{{ constructor

	/**
	 * PEAR_Error constructor
	 *
	 * @param $message error message
	 * @param $code (optional) error code
	 *
	 */
	function PEAR_Error($message = 'unknown error',
						$code = 0,
						$mode = PEAR_ERROR_RETURN,
						$level = E_USER_NOTICE)
	{
		$this->message = $message;
		$this->code    = $code;
		$this->mode    = $mode;
		$this->level   = $level;
		if (!$this->classname) {
			$this->classname = get_class($this);
		}
		switch ($this->mode) {
			case PEAR_ERROR_PRINT:
				print $this->getMessage();
				break;
			case PEAR_ERROR_TRIGGER:
				trigger_error($this->getMessage(), $this->level);
				break;
			case PEAR_ERROR_DIE:
				die($this->getMessage());
				break;
			case PEAR_ERROR_RETURN:
			default:
				break;
		}
	}

    // }}}
    // {{{ getMessage()

	
	/**
	 * Get the error message from an error object.
	 *
	 * @return string full error message
	 */
	function getMessage ()
	{
		return ($this->error_prepend . $this->error_message_prefix .
		        $this->message       . $this->error_append);
	}
	

    // }}}
    // {{{ getType()

	/**
	 * Get the name of this error/exception.
	 *
	 * @return string error/exception name (type)
	 */
	function getType ()
	{
		return $this->classname;
	}

    // }}}
    // {{{ toString()

	/**
	 * Make a string representation of this object.
	 *
	 * @return string a string with an object "summary"
	 */
	function toString() {
		$modes = array(PEAR_ERROR_RETURN => "return",
					   PEAR_ERROR_PRINT => "print",
					   PEAR_ERROR_TRIGGER => "trigger",
					   PEAR_ERROR_DIE => "die");
		$levels = array(E_USER_NOTICE => "notice",
						E_USER_WARNING => "warning",
						E_USER_ERROR => "error");
		return sprintf("[%s: message=%s code=%d mode=%s level=%s prefix=%s prepend=%s append=%s]",
					   $this->message, $this->code, $modes[$this->mode], $levels[$this->level],
					   $this->error_message_prefix, $this->error_prepend, $this->error_append);
	}

    // }}}
}

register_shutdown_function("_PEAR_call_destructors");

/*
 * Local Variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
?>
