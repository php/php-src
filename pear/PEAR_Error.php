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
// +----------------------------------------------------------------------+
//
// $Id$
//
// Commonly needed functions searching directory trees
//

//
// This class is based on ideas from Ulf Wendel
//

class PEAR_Error
{
	var $classname            = '';
	var $error_message_prefix = '';
	var $error_prepend        = '';
	var $error_append         = '';
	
	var $die_on_error         = false;
	var $auto_print_error     = false;
	
	var $level         = 0;
	var $trigger_error = false;
	
	var $message = '';
	
	/*
	 * constructor, set the basics...
	 */
	function PEAR_Error ($message)
	{
		$this->message = $message;
	}
	
	/*
	 *  string getMessage (void)
	 *    Get the error message from an exception object.
	 */
	function getMessage ()
	{
		return ($this->error_prepend . $this->error_message_prefix .
		        $this->message       . $this->error_append);
	}
	
	/*
	 * string getType (void)
	 *  Get the name of the exception.
	 */
	function getType ()
	{
		return ($this->classname);
	}
}