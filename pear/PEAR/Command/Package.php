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

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Packager.php';
require_once 'PEAR/Common.php';

class PEAR_Command_Package extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Package constructor.
     *
     * @access public
     */
    function PEAR_Command_Package(&$ui, &$config)
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
        return array('package',
                     'package-info',
                     'package-list',
                     'package-validate');
    }

    // }}}

    // {{{ getHelp()

    function getHelp($command)
    {
        switch ($command) {
            case 'package':
                return array('[<package.xml>]',
                             'Creates a PEAR package from its description file (usually '.
                             'named as package.xml)');
            case 'package-list':
                return array('<pear package>',
                             'List the contents of a PEAR package');
            case 'packge-info':
                return array('<pear package>',
                             'Shows information about a PEAR package');
            case 'package-validate':
                return array('<package.(tgz|tar|xml)>',
                             'Verifies a package or description file');
        }
    }

    // }}}

    // {{{ run()

    /**
     * Execute the command.
     *
     * @param string command name
     *
     * @param array option_name => value
     *
     * @param array list of additional parameters
     *
     * @return bool TRUE on success, FALSE for unknown commands, or
     * a PEAR error on failure
     *
     * @access public
     */
    function run($command, $options, $params)
    {
        $failmsg = '';
        switch ($command) {
            // {{{ package

            case 'package': {
                $pkginfofile = isset($params[0]) ? $params[0] : 'package.xml';
                ob_start();
                $packager =& new PEAR_Packager($this->config->get('php_dir'),
                                               $this->config->get('ext_dir'),
                                               $this->config->get('doc_dir'));
                $packager->debug = $this->config->get('verbose');
                $err = $warn = array();
                $packager->validateInfo($pkginfofile, $err, $warn);
                foreach ($err as $e) {
                    $this->ui->displayLine("Error: $e");
                }
                foreach ($warn as $w) {
                    $this->ui->displayLine("Warning: $w");
                }
                $this->ui->displayLine(sprintf('%d error(s), %d warning(s)',
                                               sizeof($err), sizeof($warn)));
                if (sizeof($err) > 0) {
                    $this->ui->displayLine("Fix these errors and try again.");
                    break;
                }
                $result = $packager->Package($pkginfofile);
                $output = ob_get_contents();
                ob_end_clean();
                $lines = explode("\n", $output);
                foreach ($lines as $line) {
                    $this->ui->displayLine($line);
                }
                if (PEAR::isError($result)) {
                    $this->ui->displayLine("Package failed!");
                } else {
                    $this->ui->displayLine("Package ok.");
                }
                break;
            }

            // }}}
            // {{{ package-list

            case 'package-list': {
                $obj = new PEAR_Common();
                if (PEAR::isError($info = $obj->infoFromTgzFile($params[0]))) {
                    return $info;
                }
                $list =$info['filelist'];
                $caption = 'Contents of ' . basename($params[0]);
                $this->ui->startTable(array('caption' => $caption,
                                            'border' => true));
                $this->ui->tableRow(array('Package Files', 'Install Destination'),
                                    array('bold' => true));
                foreach ($list as $file => $att) {
                    if (isset($att['baseinstalldir'])) {
                        $dest = $att['baseinstalldir'] . DIRECTORY_SEPARATOR .
                                basename($file);
                    } else {
                        $dest = basename($file);
                    }
                    switch ($att['role']) {
                        case 'test':
                            $dest = '-- will not be installed --'; break;
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
                    $this->ui->tableRow(array($file, $dest), null, $opts);
                }
                $this->ui->endTable();
                break;
            }

            // }}}
            // {{{ package-info

            case 'package-info': {
                $obj = new PEAR_Common();
                if (PEAR::isError($info = $obj->infoFromTgzFile($params[0]))) {
                    return $info;
                }
                unset($info['filelist']);
                unset($info['changelog']);
                $keys = array_keys($info);
                $longtext = array('description', 'summary');
                foreach ($keys as $key) {
                    if (is_array($info[$key])) {
                        switch ($key) {
                            case 'maintainers': {
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
                            }
                            case 'release_deps': {
                                static $rel_trans = array(
                                    'lt' => '<',
                                    'le' => '<=',
                                    'eq' => '=',
                                    'ne' => '!=',
                                    'gt' => '>',
                                    'ge' => '>=',
                                    );
                                $i = 0;
                                $dstr = '';
                                foreach ($info[$key] as $d) {
                                    if ($i++ > 0) {
                                        $dstr .= ", ";
                                    }
                                    if (isset($rel_trans[$d['rel']])) {
                                        $d['rel'] = $rel_trans[$d['rel']];
                                    }
                                    $dstr .= "$d[type] $d[rel]";
                                    if (isset($d['version'])) {
                                        $dstr .= " $d[version]";
                                    }
                                }
                                $info[$key] = $dstr;
                                break;
                            }
                            default: {
                                $info[$key] = implode(", ", $info[$key]);
                                break;
                            }
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
                    $key = ucwords(str_replace('_', ' ', $key));
                    $this->ui->tableRow(array($key, $value), null, array(1 => array('wrap' => 55)));
                }
                $this->ui->endTable();
                break;
            }

            // }}}
            // {{{ package-validate

            case 'package-validate': {
                if (sizeof($params) < 1) {
                    $help = $this->getHelp($command);
                    return $this->raiseError("$command: missing parameter: $help[0]");
                }
                $obj = new PEAR_Common;
                $info = null;
                $validate_result = $obj->validatePackageInfo($info, $err, $warn);
                if (file_exists($params[0])) {
                    $fp = fopen($params[0], "r");
                    $test = fread($fp, 5);
                    fclose($fp);
                    if ($test == "<?xml") {
                        $info = $obj->infoFromDescriptionFile($params[0]);
                    }
                }
                if (empty($info)) {
                    $info = $obj->infoFromTgzFile($params[0]);
                }
                if (PEAR::isError($info)) {
                    return $this->raiseError($info);
                }
                foreach ($err as $e) {
                    $this->ui->displayLine("Error: $e");
                }
                foreach ($warn as $w) {
                    $this->ui->displayLine("Warning: $w");
                }
                $this->ui->displayLine(sprintf('%d error(s), %d warning(s)',
                                               sizeof($err), sizeof($warn)));
                break;
            }

            // }}}
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