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
// | Author: Stig Bakken <ssb@fast.no>                                    |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Command/Common.php";
require_once "PEAR/Installer.php";

/**
 * PEAR commands for installation or deinstallation/upgrading of
 * packages.
 *
 */
class PEAR_Command_Install extends PEAR_Command_Common
{
    // {{{ properties
    // }}}

    // {{{ constructor

    /**
     * PEAR_Command_Install constructor.
     *
     * @access public
     */
    function PEAR_Command_Install(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ getCommands()

    /**
     * Return a list of all the commands defined by this class.
     * @return array list of commands
     * @access public
     */
    function getCommands()
    {
        return array('install', 'uninstall', 'upgrade');
    }

    function getHelp($command)
    {
        switch ($command) {
            case 'install':
                $ret = array('<pear package>',
                             'Installs a PEAR package created by the "package" command');
                break;
            case 'uninstall':
                $ret = array('<package>',
                             'Uninstalls a previously installed PEAR package');
                break;
            case 'upgrade':
                $ret = array('<pear package>',
                             'Upgrades a PEAR package installed in the system');
                break;
        }
        $ret[0] = "[-n] [-f] {$ret[0]}";
        $ret[1] = "{$ret[1]}\n" .
                  "   -f    forces the installation of the package\n".
                  "         when it is already installed\n".
                  "   -n    do not take care of package dependencies\n".
                  "   -s    soft update: install or upgrade only if needed".
                  "   -Z    no compression: download plain .tar files";
        return $ret;
    }

    // }}}
    // {{{ getOptions()

    function getOptions()
    {
        return array('f', 'n', 's', 'Z');
    }

    // }}}
    // {{{ run()

    function run($command, $options, $params)
    {
        $installer = &new PEAR_Installer($this->config->get('php_dir'),
                                         $this->config->get('ext_dir'),
                                         $this->config->get('doc_dir'));
        $installer->setFrontend($this->ui);
        $installer->debug = $this->config->get('verbose');

        $failmsg = '';
        $opts = array();
        if (isset($options['f'])) {
            $opts['force'] = true;
        }
        if (isset($options['n'])) {
            $opts['nodeps'] = true;
        }
        if (isset($options['s'])) {
            $opts['soft'] = true;
        }
        if (isset($options['Z'])) {
            $opts['nocompress'] = true;
        }
        switch ($command) {
            case 'upgrade':
                $opts['upgrade'] = true;
                // fall through
            case 'install': {
                if ($installer->install(@$params[0], $opts, $this->config)) {
                    $this->ui->displayLine("install ok");
                } else {
                    $failmsg = "install failed";
                }
                break;
            }
            case 'uninstall': {
                if ($installer->uninstall($params[0], $options)) {
                    $this->ui->displayLine("uninstall ok");
                } else {
                    $failmsg = "uninstall failed";
                }
                break;
            }
            default: {
                return false;
            }
        }
        if ($failmsg) {
            return $this->raiseError($failmsg);
        }
        return true;
    }

    // }}}
}

?>