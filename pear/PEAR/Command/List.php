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
require_once "PEAR/Registry.php";

/**
 * PEAR commands for managing configuration data.
 *
 */
class PEAR_Command_List extends PEAR_Command_Common
{
    // {{{ properties
    // }}}

    // {{{ constructor

    /**
     * PEAR_Command_Config constructor.
     *
     * @access public
     */
    function PEAR_Command_List(&$ui, &$config)
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
        return array('list');
    }

    // }}}
    // {{{ run()

    function run($command, $params)
    {
        $reg = new PEAR_Registry(); // XXX Use config here
        $installed = $reg->packageInfo();
        $i = $j = 0;
        ob_start();
        heading("Installed packages:");
        foreach ($installed as $package) {
            if ($i++ % 20 == 0) {
                if ($j++ > 0) {
                    print "\n";
                }
                printf("%-20s %-10s %s\n",
                       "Package", "Version", "State");
                print str_repeat("-", 75)."\n";
            }
            printf("%-20s %-10s %s\n", $package['package'],
                   $package['version'], $package['release_state']);
        }
        $output = ob_get_contents();
        ob_end_clean();
        $lines = explode("\n", $output);
        foreach ($lines as $line) {
            $this->ui->displayLine($line);
        }
    }
}
?>