<?php
/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Stig Sæther Bakken <ssb@fast.no>                             |
  +----------------------------------------------------------------------+

  $Id$
*/

require_once "PEAR.php";

/**
 * @nodep GtkWindow
 * @nodep gdk
 */
class PEAR_Frontend_Gtk extends PEAR
{
    // {{{ properties

    /**
     * What type of user interface this frontend is for.
     * @var string
     * @access public
     */
    var $type = 'Gtk';

    var $omode = 'plain';
    var $params = array();
    var $window = null;

    // }}}

    // {{{ constructor

    function PEAR_Frontend_Gtk()
    {
        parent::PEAR();
        if (!extension_loaded('php_gtk')) {
            dl('php_gtk.' . (OS_WINDOWS ? 'dll' : 'so'));
        }
        $this->window = &new GtkWindow();
        $this->window->set_title('PEAR Installer');
        $this->window->set_usize((gdk::screen_width()/3), (gdk::screen_height()/3));
        $this->window->show_all();
    }

    // }}}

    // {{{ displayLine(text)

    function displayLine($text)
    {
    }

    function display($text)
    {
    }

    // }}}
    // {{{ displayError(eobj)

    function displayError($eobj)
    {
    }

    // }}}
    // {{{ displayFatalError(eobj)

    function displayFatalError($eobj)
    {
    }

    // }}}
    // {{{ displayHeading(title)

    function displayHeading($title)
    {
    }

    // }}}
    // {{{ userDialog(prompt, [type], [default])

    function userDialog($prompt, $type = 'text', $default = '')
    {
    }

    // }}}
    // {{{ userConfirm(prompt, [default])

    function userConfirm($prompt, $default = 'yes')
    {
    }

    // }}}
    // {{{ startTable([params])

    function startTable($params = array())
    {
    }

    // }}}
    // {{{ tableRow(columns, [rowparams], [colparams])

    function tableRow($columns, $rowparams = array(), $colparams = array())
    {
    }

    // }}}
    // {{{ endTable()

    function endTable()
    {
    }

    // }}}
    // {{{ bold($text)

    function bold($text)
    {
    }

    // }}}
}

?>
