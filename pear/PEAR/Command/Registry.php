<?php
// /* vim: set expandtab tabstop=4 shiftwidth=4: */
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
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Registry.php';
require_once 'PEAR/Config.php';

class PEAR_Command_Registry extends PEAR_Command_Common
{
    var $commands = array(
        'list' => array(
            'summary' => 'List Installed Packages',
            'function' => 'doList',
            'shortcut' => 'l',
            'options' => array(),
            'doc' => '[package]
If invoked without parameters, this command lists the PEAR packages
installed in your php_dir ({config php_dir)).  With a parameter, it
lists the files in that package.
',
            ),
        'shell-test' => array(
            'summary' => 'Shell Script Test',
            'function' => 'doShellTest',
            'shortcut' => 'st',
            'options' => array(),
            'doc' => '<package> [[relation] version]
Tests if a package is installed in the system. Will exit(1) if it is not.
   <relation>   The version comparison operator. One of:
                <, lt, <=, le, >, gt, >=, ge, ==, =, eq, !=, <>, ne
   <version>    The version to compare with
'),
        );

    /**
     * PEAR_Command_Registry constructor.
     *
     * @access public
     */
    function PEAR_Command_Registry(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    function doList($command, $options, $params)
    {
        $reg = new PEAR_Registry($this->config->get('php_dir'));
        if (sizeof($params) == 0) {
            $installed = $reg->packageInfo();
            $i = $j = 0;
            $this->ui->startTable(
                array('caption' => 'Installed packages:',
                      'border' => true));
            foreach ($installed as $package) {
                if ($i++ % 20 == 0) {
                    $this->ui->tableRow(
                        array('Package', 'Version', 'State'),
                        array('bold' => true));
                }
                $this->ui->tableRow(array($package['package'],
                                          $package['version'],
                                          @$package['release_state']));
            }
            if ($i == 0) {
                $this->ui->tableRow(array('(no packages installed)'));
            }
            $this->ui->endTable();
        } else {
            if (file_exists($params[0]) && !is_dir($params[0])) {
                include_once "PEAR/Common.php";
                $obj = &new PEAR_Common;
                $info = $obj->infoFromAny($params[0]);
                $headings = array('Package File', 'Install Path');
                $installed = false;
            } else {
                $info = $reg->packageInfo($params[0]);
                $headings = array('Type', 'Install Path');
                $installed = true;
            }
            if (PEAR::isError($info)) {
                return $this->raiseError($info);
            }
            if ($info === null) {
                return $this->raiseError("`$params[0]' not installed");
            }
            $list = $info['filelist'];
            $caption = 'Contents of ' . basename($params[0]);
            $this->ui->startTable(array('caption' => $caption,
                                        'border' => true));
            $this->ui->tableRow($headings, array('bold' => true));
            foreach ($list as $file => $att) {
                if (isset($att['baseinstalldir'])) {
                    $dest = $att['baseinstalldir'] . DIRECTORY_SEPARATOR .
                        $file;
                } else {
                    $dest = $file;
                }
                switch ($att['role']) {
                    case 'test':
                    case 'data':
                        if ($installed) {
                            break 2;
                        }
                        $dest = '-- will not be installed --';
                        break;
                    case 'doc':
                        $dest = $this->config->get('doc_dir') . DIRECTORY_SEPARATOR .
                            $dest;
                        break;
                    case 'php':
                    default:
                        $dest = $this->config->get('php_dir') . DIRECTORY_SEPARATOR .
                            $dest;
                }
                $dest = preg_replace('!/+!', '/', $dest);
                $file = preg_replace('!/+!', '/', $file);
                $opts = array(0 => array('wrap' => 23),
                              1 => array('wrap' => 45)
                    );
                if ($installed) {
                    $this->ui->tableRow(array($att['role'], $dest), null, $opts);
                } else {
                    $this->ui->tableRow(array($file, $dest), null, $opts);
                }
            }
            $this->ui->endTable();

            
        }
        return true;
    }

    function doShellTest($command, $options, $params) {
        $this->pushErrorHandling(PEAR_ERROR_RETURN);
        $reg = &new PEAR_Registry($this->config->get('php_dir'));
        // "pear shell-test Foo"
        if (sizeof($params) == 1) {
            if (!$reg->packageExists($params[0])) {
                exit(1);
            }
            // "pear shell-test Foo 1.0"
        } elseif (sizeof($params) == 2) {
            $v = $reg->packageInfo($params[0], 'version');
            if (!$v || !version_compare($v, $params[1], "ge")) {
                exit(1);
            }
            // "pear shell-test Foo ge 1.0"
        } elseif (sizeof($params) == 3) {
            $v = $reg->packageInfo($params[0], 'version');
            if (!$v || !version_compare($v, $params[2], $params[1])) {
                exit(1);
            }
        } else {
            $this->popErrorHandling();
            $this->raiseError("$command: expects 1 to 3 parameters");
            exit(1);
        }
    }

}

?>