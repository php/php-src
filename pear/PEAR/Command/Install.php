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
     * PEAR_Command_Install constructor.
     *
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
        $installer =& new PEAR_Installer($options['php_dir'],
                                         $options['ext_dir'],
                                         $options['doc_dir']);
        $installer->debug = @$options['verbose'];
        $status = PEAR_COMMAND_SUCCESS;
        ob_start();
        switch ($command) {
            case 'install':
            case 'upgrade': {
                if ($command == 'upgrade') {
                    $options['upgrade'] = true;
                }
                if ($installer->install($params[0], $options, $this->config)) {
                    print "install ok\n";
                } else {
                    print "install failed\n";
                    $status = PEAR_COMMAND_FAILURE;
                }
                break;
            }
            case 'uninstall': {
                if ($installer->uninstall($params[0], $uninstall_options)) {
                    print "uninstall ok\n";
                } else {
                    print "uninstall failed\n";
                    $status = PEAR_COMMAND_FAILURE;
                }
                break;
            }                
        }
        $output = ob_get_contents();
        ob_end_clean();
        return $this->makeResponse($status, $output);
    }
}

?>