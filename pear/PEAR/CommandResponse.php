<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR.php";

define('PEAR_COMMAND_FAILURE', 0);
define('PEAR_COMMAND_SUCCESS', 1);
define('PEAR_COMMAND_PARTIAL', 2);

/**
 * PEAR_CommandResponse is for returning an "environment-neutral"
 * response to the application when a PEAR command is done.  This
 * means that there should be no HTML markup etc. in the message.  The
 * application should try rendering the message "as-is" with
 * linebreaks and preferably a fixed-width font.
 */
class PEAR_CommandResponse extends PEAR
{
    /** Status code (one of the PEAR_COMMAND_* constants
     * @var int
     */
    var $status = null;

    /** Message for user, in plain text.
     * @var string
     */
    var $message = '';

    /** Character set/encoding of $message.
     * @var string
     */
    var $encoding = 'ISO-8859-1';

    /**
     * Constructor.  Not very exciting.
     *
     * @param int     Command status, one of:
     *                PEAR_COMMAND_SUCCESS : the command was successful
     *                PEAR_COMMAND_FAILURE : the command failed
     *                PEAR_COMMAND_PARTIAL : the command was successful,
     *                but requires some other command to complete the
     *                operation (for example if the user must confirm
     *                something).
     * @param string  Message for the user.
     * @param string  (optional) What character encoding the message
     *                is in, defaults to ISO-8859-1.
     * @access public
     */
    function PEAR_CommandRepsonse($status, $message, $encoding = null)
    {
        if ($encoding !== null) {
            $this->setEncoding($encoding);
        }
        $this->status = $status;
        $this->message = $message;
    }

    /**
     * Get the response status code.
     * @return int response status code
     * @access public
     */
    function getStatus()
    {
        return $this->status;
    }

    /**
     * Get the response message.
     * @return string response message
     * @access public
     */
    function getMessage()
    {
        return $this->message;
    }

    /**
     * Get the response message charset encoding.
     * @return string response message charset encoding
     * @access public
     */
    function getEncoding()
    {
        return $this->encoding;
    }

    /**
     * Set the response message charset encoding.
     * @param string Which encoding to use, valid charsets are currently
     *               ISO-8859-{1-15} and UTF-8.
     * @return bool true if the encoding was valid, false if not
     * @access public
     */
    function setEncoding($encoding)
    {
        if (preg_match('/^ISO-8859-([1-9]|1[1-5])$/i', $encoding) ||
            strcasecmp('UTF-8', $encoding) == 0) {
            $this->encoding = $encoding;
            return true;
        }
        return false;
    }
}

?>