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
        'info' => array(
            'summary' => 'Display information about a package',
            'function' => 'doInfo',
            'options' => array(),
            'doc' => '<pacakge>
Displays information about a package.  The package argument may be a
local package file, an URL to a package file, or the name of an installed
package.',
            ),
        'list' => array(
            'summary' => 'List Installed Packages',
            'function' => 'doList',
            'shortcut' => 'l',
            'options' => array(),
            'doc' => '[package]
If invoked without parameters, this command lists the PEAR packages
installed in your php_dir ({config php_dir)).  With a parameter, it
lists the files in that package.',
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
   <version>    The version to compare with',
            ),
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

    function doInfo($command, $options, $params)
    {
        // $params[0] -> the PEAR package to list its information
        if (sizeof($params) != 1) {
            return $this->raiseError("bad parameter(s), try \"help $command\"");
        }

        if (file_exists($params[0]) && !is_dir($params[0])) {
            $obj = &new PEAR_Common();
            $info = $obj->infoFromAny($params[0]);
        } else {
            $reg = &new PEAR_Registry($this->config->get('php_dir'));
            $info = $reg->packageInfo($params[0]);
        }

        if (PEAR::isError($info)) {
            return $info;
        }
        if (empty($info)) {
            $this->ui->displayLine("Nothing found for `$params[0]'");
            return;
        }

        unset($info['filelist']);
        unset($info['changelog']);
        $keys = array_keys($info);
        $longtext = array('description', 'summary');
        foreach ($keys as $key) {
            if (is_array($info[$key])) {
                switch ($key) {
                    case 'maintainers':
                        $i = 0;
                        $mstr = '';
                        foreach ($info[$key] as $m) {
                            if ($i++ > 0) {
                                $mstr .= "\n";
                            }
                            $mstr .= $m['name'] . " <";
                            if (isset($m['email'])) {
                                $mstr .= $m['email'];
                            } else {
                                $mstr .= $m['handle'] . '@php.net';
                            }
                            $mstr .= "> ($m[role])";
                        }
                        $info[$key] = $mstr;
                        break;
                    case 'release_deps':
                        $i = 0;
                        $dstr = '';
                        foreach ($info[$key] as $d) {
                            if ($i++ > 0) {
                                $dstr .= ", ";
                            }
                            if (isset($this->_deps_rel_trans[$d['rel']])) {
                                $d['rel'] = $this->_deps_rel_trans[$d['rel']];
                            }
                            $dstr .= "$d[type] $d[rel]";
                            if (isset($d['version'])) {
                                $dstr .= " $d[version]";
                            }
                        }
                        $info[$key] = $dstr;
                        break;
                    default:
                        $info[$key] = implode(", ", $info[$key]);
                        break;
                }
            }
            $info[$key] = trim($info[$key]);
            if (in_array($key, $longtext)) {
                $info[$key] = preg_replace('/  +/', ' ', $info[$key]);
            }
        }
        $caption = 'About ' . basename($params[0]);
        $this->ui->startTable(array('caption' => $caption,
                                    'border' => true));
        foreach ($info as $key => $value) {
            if ($key{0} == '_') continue;
            $key = ucwords(str_replace('_', ' ', $key));
            $this->ui->tableRow(array($key, $value), null, array(1 => array('wrap' => 55)));
        }
        $this->ui->endTable();
        return true;
    }

    function doList($command, $options, $params)
    {
        $reg = &new PEAR_Registry($this->config->get('php_dir'));
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
            if ($installed) {
                $caption = 'Installed Files For ' . $params[0];
            } else {
                $caption = 'Contents of ' . basename($params[0]);
            }
            $this->ui->startTable(array('caption' => $caption,
                                        'border' => true));
            $this->ui->tableRow($headings, array('bold' => true));
            foreach ($list as $file => $att) {
                if ($installed) {
                    if (empty($att['installed_as'])) {
                        continue;
                    }
                    $this->ui->tableRow(array($att['role'], $att['installed_as']));
                } else {
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
                    $this->ui->tableRow(array($file, $dest));
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