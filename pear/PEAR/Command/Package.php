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
// |          Martin Jansen <mj@php.net>                                  |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Common.php';
require_once 'PEAR/Command/Common.php';

class PEAR_Command_Package extends PEAR_Command_Common
{
    var $_deps_rel_trans = array(
                                 'lt' => '<',
                                 'le' => '<=',
                                 'eq' => '=',
                                 'ne' => '!=',
                                 'gt' => '>',
                                 'ge' => '>=',
                                 );
    var $_deps_type_trans = array(
                                  'pkg' => 'package',
                                  'extension' => 'extension',
                                  'php' => 'PHP',
                                  'prog' => 'external program',
                                  'ldlib' => 'external library for linking',
                                  'rtlib' => 'external runtime library',
                                  'os' => 'operating system',
                                  'websrv' => 'web server',
                                  'sapi' => 'SAPI backend'
                                  );
    var $commands = array(
        'package' => array(
            'summary' => 'Build Package',
            'function' => 'doPackage',
            'shortcut' => 'p',
            'options' => array(
                'nocompress' => array(
                    'shortopt' => 'Z',
                    'doc' => 'Do not gzip the package file'
                    ),
                'showname' => array(
                    'shortopt' => 'n',
                    'doc' => 'Print the name of the packaged file.',
                    ),
                ),
            'doc' => '[descfile]
Creates a PEAR package from its description file (usually called
package.xml).
'
            ),
        'package-validate' => array(
            'summary' => 'Validate Package Consistency',
            'function' => 'doPackageValidate',
            'shortcut' => 'pv',
            'options' => array(),
            'doc' => '
',
            ),
        'cvstag' => array(
            'summary' => 'Set CVS Release Tag',
            'function' => 'doCvsTag',
            'shortcut' => 'ct',
            'options' => array(
                'quiet' => array(
                    'shortopt' => 'q',
                    'doc' => 'Be quiet',
                    ),
                'reallyquiet' => array(
                    'shortopt' => 'Q',
                    'doc' => 'Be really quiet',
                    ),
                'slide' => array(
                    'shortopt' => 'F',
                    'doc' => 'Move (slide) tag if it exists',
                    ),
                'delete' => array(
                    'shortopt' => 'd',
                    'doc' => 'Remove tag',
                    ),
                ),
            'doc' => '
Sets a CVS tag on all files in a package.  Use this command after you have
packaged a distribution tarball with the "package" command to tag what
revisions of what files were in that release.  If need to fix something
after running cvstag once, but before the tarball is released to the public,
use the "slide" option to move the release tag.
',
            ),
        'run-tests' => array(
            'summary' => 'Run Regression Tests',
            'function' => 'doRunTests',
            'shortcut' => 'rt',
            'options' => array(),
            'doc' => '[testfile|dir ...]
Run regression tests with PHP\'s regression testing script (run-tests.php).',
            ),
        'package-dependencies' => array(
                                        'summary' => 'Show package dependencies',
                                        'function' => 'doPackageDependencies',
                                        'shortcut' => 'pd',
                                        'options' => array(),
                                        'doc' => '
List all depencies the package has.'
                                        ),    
        );

    /**
     * PEAR_Command_Package constructor.
     *
     * @access public
     */
    function PEAR_Command_Package(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    function _displayValidationResults($err, $warn, $strict = false)
    {
        foreach ($err as $e) {
            $this->ui->displayLine("Error: $e");
        }
        foreach ($warn as $w) {
            $this->ui->displayLine("Warning: $w");
        }
        $this->ui->displayLine(sprintf('Validation: %d error(s), %d warning(s)',
                                       sizeof($err), sizeof($warn)));
        if ($strict && sizeof($err) > 0) {
            $this->ui->displayLine("Fix these errors and try again.");
            return false;
        }
        return true;
    }

    function doPackage($command, $options, $params)
    {
        include_once 'PEAR/Packager.php';
        $pkginfofile = isset($params[0]) ? $params[0] : 'package.xml';
        ob_start();
        $packager =& new PEAR_Packager($this->config->get('php_dir'),
                                       $this->config->get('ext_dir'),
                                       $this->config->get('doc_dir'));
        $packager->debug = $this->config->get('verbose');
        $err = $warn = array();
        $packager->validatePackageInfo($pkginfofile, $err, $warn);
        if (!$this->_displayValidationResults($err, $warn, true)) {
            return;
        }
        $compress = empty($options['Z']) ? true : false;
        $result = $packager->Package($pkginfofile, $compress);
        $output = ob_get_contents();
        ob_end_clean();
        if (PEAR::isError($result)) {
            return $this->raiseError($result);
        }
        // Don't want output, only the package file name just created
        if (isset($options['n'])) {
            $this->ui->displayLine($result);
            return;
        }
        $lines = explode("\n", $output);
        foreach ($lines as $line) {
            $this->ui->displayLine($line);
        }
        if (PEAR::isError($result)) {
            $this->ui->displayLine("Package failed: ".$result->getMessage());
        }
        return true;
    }

    function doPackageValidate($command, $options, $params)
    {
        if (sizeof($params) < 1) {
            $params[0] = "package.xml";
        }
        $obj = new PEAR_Common;
        $info = null;
        if ($fp = @fopen($params[0], "r")) {
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
        $obj->validatePackageInfo($info, $err, $warn);
        $this->_displayValidationResults($err, $warn);
        return true;
    }

    function doCvsTag($command, $options, $params)
    {
        if (sizeof($params) < 1) {
            $help = $this->getHelp($command);
            return $this->raiseError("$command: missing parameter: $help[0]");
        }
        $obj = new PEAR_Common;
        $info = $obj->infoFromDescriptionFile($params[0]);
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }
        $err = $warn = array();
        $obj->validatePackageInfo($info, $err, $warn);
        if (!$this->_displayValidationResults($err, $warn, true)) {
            break;
        }
        $version = $info['version'];
        $cvsversion = preg_replace('/[^a-z0-9]/i', '_', $version);
        $cvstag = "RELEASE_$cvsversion";
        $files = array_keys($info['filelist']);
        $command = "cvs";
        if (isset($options['quiet'])) {
            $command .= ' -q';
        }
        if (isset($options['reallyquiet'])) {
            $command .= ' -Q';
        }
        $command .= ' tag';
        if (isset($options['slide'])) {
            $command .= ' -F';
        }
        if (isset($options['delete'])) {
            $command .= ' -d';
        }
        $command .= ' ' . $cvstag . ' ' . escapeshellarg($params[0]);
        foreach ($files as $file) {
            $command .= ' ' . escapeshellarg($file);
        }
        $this->ui->displayLine("+ $command");
        if (empty($options['n'])) {
            $fp = popen($command, "r");
            while ($line = fgets($fp, 1024)) {
                $this->ui->displayLine(rtrim($line));
            }
            pclose($fp);
        }
        return true;
    }

    function doRunTests($command, $options, $params)
    {
        $cwd = getcwd();
        $php = PHP_BINDIR . '/php' . (OS_WINDOWS ? '.exe' : '');
        putenv("TEST_PHP_EXECUTABLE=$php");
        $ip = ini_get("include_path");
        $ps = OS_WINDOWS ? ';' : ':';
        $run_tests = $this->config->get('php_dir') . DIRECTORY_SEPARATOR . 'run-tests.php';
        if (!file_exists($run_tests)) {
            $run_tests = PEAR_INSTALL_DIR . DIRECTORY_SEPARATOR . 'run-tests.php';
            if (!file_exists($run_tests)) {
                return $this->raiseError("No `run-test.php' file found");
            }
        }
        $plist = implode(" ", $params);
        $cmd = "$php -C -d include_path=$cwd$ps$ip -f $run_tests -- $plist";
        system($cmd);
        return true;
    }

    function doPackageDependencies($command, $options, $params)
    {
        // $params[0] -> the PEAR package to list its information
        if (sizeof($params) != 1) {
            return $this->raiseError("bad parameter(s), try \"help $command\"");
        }

        $obj = new PEAR_Common();
        if (PEAR::isError($info = $obj->infoFromAny($params[0]))) {
            return $info;
        }

        if (is_array($info['release_deps'])) {
            $this->ui->startTable(array('caption' => 'Dependencies for ' . $info['package'],
                                        'border' => true));
            $this->ui->tableRow(array("Type", "Name", "Relation", "Version"),
                                array('bold' => true));

            foreach ($info['release_deps'] as $d) {

                if (isset($this->_deps_rel_trans[$d['rel']])) {
                    $rel = $this->_deps_rel_trans[$d['rel']];
                } else {
                    $rel = $d['rel'];
                }

                if (isset($this->_deps_type_trans[$d['type']])) {
                    $type = ucfirst($this->_deps_type_trans[$d['type']]);
                } else {
                    $type = $d['type'];
                }

                if (isset($d['name'])) {
                    $name = $d['name'];
                } else {
                    $name = '';
                }

                if (isset($d['version'])) {
                    $version = $d['version'];
                } else {
                    $version = '';
                }

                $this->ui->tableRow(array($type, $name, $rel, $version), null, array(1 => array('wrap' => 55)));
            }

            $this->ui->endTable();
            return true;
        }

        // Fallback
        $this->ui->displayLine("This package does not have any dependencies.");
    }
}
?>