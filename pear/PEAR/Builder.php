<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
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

    // used for msdev builds
    var $_lastline = null;
    var $_firstline = null;
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

    // }}}

    // {{{ _build_win32()

    /**
     * Build an extension from source on windows.
     * requires msdev
     */
    function _build_win32($descfile, $callback = null)
    {
        if (PEAR::isError($info = $this->infoFromDescriptionFile($descfile))) {
            return $info;
        }
        $dir = dirname($descfile);
        $old_cwd = getcwd();

        if (!@chdir($dir)) {
            return $this->raiseError("could not chdir to $dir");
        }
        $this->log(2, "building in $dir");

        $dsp = $info['package'].'.dsp';
        if (!@is_file("$dir/$dsp")) {
            return $this->raiseError("The DSP $dsp does not exist.");
        }
        // XXX TODO: make release build type configurable
        $command = 'msdev '.$dsp.' /MAKE "'.$info['package']. ' - Release"';

        $this->current_callback = $callback;
        $err = $this->_runCommand($command, array(&$this, 'msdevCallback'));
        if (PEAR::isError($err)) {
            return $err;
        }

        // figure out the build platform and type
        $platform = 'Win32';
        $buildtype = 'Release';
        if (preg_match('/.*?'.$info['package'].'\s-\s(\w+)\s(.*?)-+/i',$this->_firstline,$matches)) {
            $platform = $matches[1];
            $buildtype = $matches[2];
        }

        if (preg_match('/(.*)?\s-\s(\d+).*?(\d+)/',$this->_lastline,$matches)) {
            if ($matches[2]) {
                // there were errors in the build
                return $this->raiseError("There were errors during compilation.");
            }
            $out = $matches[1];
        } else {
            return $this->raiseError("Did not understand the completion status returned from msdev.exe.");
        }

        // msdev doesn't tell us the output directory :/
        // open the dsp, find /out and use that directory
        $dsptext = join(file($dsp),'');

        // this regex depends on the build platform and type having been
        // correctly identified above.
        $regex ='/.*?!IF\s+"\$\(CFG\)"\s+==\s+("'.
                    $info['package'].'\s-\s'.
                    $platform.'\s'.
                    $buildtype.'").*?'.
                    '\/out:"(.*?)"/is';

        if ($dsptext && preg_match($regex,$dsptext,$matches)) {
            // what we get back is a relative path to the output file itself.
            $outfile = realpath($matches[2]);
        } else {
            return $this->raiseError("Could not retrieve output information from $dsp.");
        }
        if (@copy($outfile, "$dir/$out")) {
            $outfile = "$dir/$out";
        }

        $built_files[] = array(
            'file' => "$outfile",
            'php_api' => $this->php_api_version,
            'zend_mod_api' => $this->zend_module_api_no,
            'zend_ext_api' => $this->zend_extension_api_no,
            );

        return $built_files;
    }
    // }}}

    // {{{ msdevCallback()
    function msdevCallback($what, $data)
    {
        if (!$this->_firstline)
            $this->_firstline = $data;
        $this->_lastline = $data;
    }
    // }}}

    // {{{ build()

    /**
     * Build an extension from source.  Runs "phpize" in the source
     * directory, but compiles in a temporary directory
     * (/var/tmp/pear-build-USER/PACKAGE-VERSION).
     *
     * @param string $descfile path to XML package description file
     *
     * @param mixed $callback callback function used to report output,
     * see PEAR_Builder::_runCommand for details
     *
     * @return array an array of associative arrays with built files,
     * format:
     * array( array( 'file' => '/path/to/ext.so',
     *               'php_api' => YYYYMMDD,
     *               'zend_mod_api' => YYYYMMDD,
     *               'zend_ext_api' => YYYYMMDD ),
     *        ... )
     *
     * @access public
     *
     * @see PEAR_Builder::_runCommand
     * @see PEAR_Common::infoFromDescriptionFile
     */
    function build($descfile, $callback = null)
    {
        if (PEAR_OS == "Windows") {
            return $this->_build_win32($descfile,$callback);
        }
        if (PEAR_OS != 'Unix') {
            return $this->raiseError("building extensions not supported on this platform");
        }
        if (PEAR::isError($info = $this->infoFromDescriptionFile($descfile))) {
            return $info;
        }
        $dir = dirname($descfile);
        $old_cwd = getcwd();
        if (!@chdir($dir)) {
            return $this->raiseError("could not chdir to $dir");
        }
        $vdir = "$info[package]-$info[version]";
        if (is_dir($vdir)) {
            chdir($vdir);
        }
        $dir = getcwd();
        $this->log(2, "building in $dir");
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
        if(!$user=getenv('USER')){
            $user='defaultuser';
        }
        $build_basedir = "/var/tmp/pear-build-$user";
        $build_dir = "$build_basedir/$info[package]-$info[version]";
        $this->log(1, "building in $build_dir");
        if (is_dir($build_dir)) {
            System::rm("-rf $build_dir");
        }
        if (!System::mkDir("-p $build_dir")) {
            return $this->raiseError("could not create build dir: $build_dir");
        }
        $this->addTempFile($build_dir);
        if (getenv('MAKE')) {
            $make_command = getenv('MAKE');
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
        $built_files = array();
        while ($ent = readdir($dp)) {
            if ($ent{0} == '.' || substr($ent, -3) == '.la') {
                continue;
            }
            // harvest!
            if (@copy("modules/$ent", "$dir/$ent")) {
                $built_files[] = array(
                    'file' => "$dir/$ent",
                    'php_api' => $this->php_api_version,
                    'zend_mod_api' => $this->zend_module_api_no,
                    'zend_ext_api' => $this->zend_extension_api_no,
                    );

                $this->log(1, "$ent copied to $dir/$ent");
            } else {
                chdir($old_cwd);
                return $this->raiseError("failed copying $ent to $dir");
            }
        }
        closedir($dp);
        chdir($old_cwd);
        return $built_files;
    }

    // }}}
    // {{{ phpizeCallback()

    /**
     * Message callback function used when running the "phpize"
     * program.  Extracts the API numbers used.  Ignores other message
     * types than "cmdoutput".
     *
     * @param string $what the type of message
     * @param mixed $data the message
     *
     * @return void
     *
     * @access public
     */
    function phpizeCallback($what, $data)
    {
        if ($what != 'cmdoutput') {
            return;
        }
        $this->log(3, rtrim($data));
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

    // }}}
    // {{{ _runCommand()

    /**
     * Run an external command, using a message callback to report
     * output.  The command will be run through popen and output is
     * reported for every line with a "cmdoutput" message with the
     * line string, including newlines, as payload.
     *
     * @param string $command the command to run
     *
     * @param mixed $callback (optional) function to use as message
     * callback
     *
     * @return bool whether the command was successful (exit code 0
     * means success, any other means failure)
     *
     * @access private
     */
    function _runCommand($command, $callback = null)
    {
        $this->log(1, "running: $command");
        $pp = @popen("$command 2>&1", "r");
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

    // }}}
    // {{{ log()

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

    // }}}
}

?>
