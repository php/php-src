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
    /** Stack of executing commands, to make run() re-entrant
     * @var array
     */
    var $command_stack; // XXX UNUSED to make run() re-entrant

    /** Currently executing command.
     * @var string
     */
    var $command; // XXX UNUSED

    /**
     * PEAR_Command_Install constructor.  Nothing to see here.
     * @access public
     */
    function PEAR_Command_Install()
    {
        parent::PEAR_Command_Common();
    }

    /**
     * Return a list of all the commands defined by this class.
     * @return array list of commands
     * @access public
     */
    function getCommands()
    {
        return array('install', 'uninstall', 'upgrade');
    }

    function run($command, $options, $params)
    {
        switch ($command) {
        }
    }

    function doInstall($command, $options, $params)
    {
            $pkgfile = $cmdargs[0];
            $installer =& new PEAR_Installer($script_dir, $ext_dir, $doc_dir);
            $installer->setErrorHandling(PEAR_ERROR_DIE,
                                         basename($pkgfile) . ": %s\n");
            $installer->debug = $verbose;
            $install_options = array();
            if ($command == 'upgrade') {
                $install_options['upgrade'] = true;
            }
            foreach ($cmdopts as $opt) {
                switch ($opt[0]) {
                    case 'r':
                        // This option is for use by rpm and other package
                        // tools that can install files etc. by itself, but
                        // still needs to register the package as installed in
                        // PEAR's local registry.
                        $install_options['register_only'] = true;
                    break;
                    case 'f':
                        $install_options['force'] = true;
                    break;
                }
            }
            if ($installer->install($pkgfile, $install_options, $config)) {
                print "install ok\n";
            } else {
                print "install failed\n";
            }
        }
    }

    function getStatus(
}

?>