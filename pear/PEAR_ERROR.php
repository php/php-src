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

class PEAR_ERROR
{
	var $CLASSNAME            = '';
	var $ERROR_MESSAGE_PREFIX = '';
	var $ERROR_PREPEND        = '';
	var $ERROR_APPEND         = '';
	
	var $DIE_ON_ERROR         = '';
	var $AUTO_PRINT_ERROR     = '';
	
	var $LEVEL         = 0;
	var $TRIGGER_ERROR = false;
	
	var $message = '';
	var $file    = '';
	var $line    = 0;
	
	function PEAR_ERROR ($message, $file = __FILE__, $line = __LINE__)
	{
		$this->message = $message;
		$this->file    = $file;
		$this->line    = $line;
	}
	
	function getMessage ()
	{
		return ($this->ERROR_PREPEND . $this->ERROR_MESSAGE_PREFIX .
		        $this->message       . $this->ERROR_APPEND);
	}
	
	function getType ()
	{
		return ($this->CLASSNAME);
	}
}