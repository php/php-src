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
// | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Common.php';

/**
 * Class to handle building (compiling) extensions.
 *
 * @author Stig Sæther Bakken <ssb@fast.no>
 */
class PEAR_Builder extends PEAR_Common
{
    // {{{ properties

    var $php_api_version = 0;
    var $zend_module_api_no = 0;
    var $zend_extension_api_no = 0;

    var $extensions_built = array();

    var $current_callback = null;

    // }}}

    // {{{ constructor

    /**
     * PEAR_Builder constructor.
     *
     * @param object $ui user interface object (instance of PEAR_Frontend_*)
     *
     * @access public
     */
    function PEAR_Builder(&$ui)
    {
        parent::PEAR_Common();
        $this->setFrontendObject($ui);
    }

    function build($descfile, $callback = null)
    {
        if (PEAR::isError($info = $this->infoFromDescriptionFile($descfile))) {
            return $info;
        }
        $dir = dirname($descfile);
        $old_cwd = getcwd();
        if (!@chdir($dir)) {
            return $this->raiseError("could not chdir to $dir");
        }
        $dir = getcwd();
        $this->current_callback = $callback;
        $err = $this->_runCommand("phpize", array(&$this, 'phpizeCallback'));
        if (PEAR::isError($err)) {
            return $err;
        }
        if (!$err) {
            return $this->raiseError("`phpize' failed");
        }
        
        // start of interactive part
        $configure_command = "$dir/configure";
        if (isset($info['configure_options'])) {
            foreach ($info['configure_options'] as $o) {
                list($r) = $this->ui->userDialog('build',
                                                 array($o['prompt']),
                                                 array('text'),
                                                 array(@$o['default']));
                if (substr($o['name'], 0, 5) == 'with-' &&
                    ($r == 'yes' || $r == 'autodetect')) {
                    $configure_command .= " --$o[name]";
                } else {
                    $configure_command .= " --$o[name]=$r";
                }
            }
        }
        // end of interactive part
        
        // make configurable
        $build_basedir = "/var/tmp/pear-build-$_ENV[USER]";
        $build_dir = "$build_basedir/$info[package]-$info[version]";
        $this->log(1, "building in $build_dir");
        if (PEAR::isError($err = System::rm("-rf $build_dir"))) {
            return $err;
        }
        if (!System::mkDir("-p $build_dir")) {
            return $this->raiseError("could not create build dir: $build_dir");
        }

        if (isset($_ENV['MAKE'])) {
            $make_command = $_ENV['MAKE'];
        } else {
            $make_command = 'make';
        }
        $to_run = array(
            $configure_command,
            $make_command,
            );
        if (!@chdir($build_dir)) {
            return $this->raiseError("could not chdir to $build_dir");
        }
        foreach ($to_run as $cmd) {
            $err = $this->_runCommand($cmd, $callback);
            if (PEAR::isError($err) && !$err) {
                chdir($old_cwd);
                return $err;
            }
        }
        if (!($dp = opendir("modules"))) {
            chdir($old_cwd);
            return $this->raiseError("no `modules' directory found");
        }
        while ($ent = readdir($dp)) {
            if ($ent{0} == '.' || substr($ent, -3) == '.la') {
                continue;
            }
            // harvest!
            if (@copy("modules/$ent", "$dir/$ent")) {
                $this->log(1, "$ent copied to $dir/$ent");
            } else {
                chdir($old_cwd);
                return $this->raiseError("failed copying $ent to $dir");
            }
        }
        closedir($dp);
        chdir($old_cwd);
        return true;
    }

    // }}}

    function phpizeCallback($what, $data)
    {
        if ($what != 'cmdoutput') {
            return;
        }
        if (preg_match('/You should update your .aclocal.m4/', $data)) {
            return;
        }
        $matches = array();
        if (preg_match('/^\s+(\S[^:]+):\s+(\d{8})/', $data, $matches)) {
            $member = preg_replace('/[^a-z]/', '_', strtolower($matches[1]));
            $apino = (int)$matches[2];
            if (isset($this->$member)) {
                $this->$member = $apino;
                $msg = sprintf("%-22s : %d", $matches[1], $apino);
                $this->log(1, $msg);
            }
        }
    }

    function _runCommand($command, $callback = null)
    {
        $this->log(1, "running: $command");
        $pp = @popen($command, "r");
        if (!$pp) {
            return $this->raiseError("failed to run `$command'");
        }
        while ($line = fgets($pp, 1024)) {
            if ($callback) {
                call_user_func($callback, 'cmdoutput', $line);
            } else {
                $this->log(2, rtrim($line));
            }
        }
        $exitcode = @pclose($pp);
        return ($exitcode == 0);
    }

    function log($level, $msg)
    {
        if ($this->current_callback) {
            if ($this->debug >= $level) {
                call_user_func($this->current_callback, 'output', $msg);
            }
            return;
        }
        return PEAR_Common::log($level, $msg);
    }
}

?>
