<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
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
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';

/**
 * This is a class for storing simple configuration values keeping
 * track of which are system-defined (defaulted) and which are
 * user-defined.  By default, only used-defined settings are stored
 * back to the user's configuration file.
 *
 * Configuration member is a simple associative array.  Used keys:
 *
 *  master_server      which server to query for mirror lists etc.
 *  server             which server/mirror we're currently using
 *  username           PEAR username
 *  password           PEAR password (stored base64-encoded)
 *  php_dir            Where to install .php files
 *  ext_dir            Directory to install compiled libs in
 *  doc_dir            Directory to install documentation in
 *
 */
class PEAR_Config extends PEAR
{
    // {{{ properties

    var $defaults_file = '';

    var $config_file = '';

    var $configuration = array();

    var $defaulted = array();

    // }}}

    // {{{ PEAR_Config([file], [defaults_file])

    function PEAR_Config($file = '', $defaults_file = '')
    {
        $this->PEAR();
        $this->config_file = $file;
        $this->defaults_file = $defaults_file;
        if ($file && file_exists($file)) {
            $this->readConfigFile($file);
        }
        if ($defaults_file && file_exists($defaults_file)) {
            $this->mergeConfigFile($defaults_file, false, true);
        }
    }

    // }}}

    // {{{ readConfigFile([file], [defaults])

    function readConfigFile($file = null, $defaults = false)
    {
        if ($file === null) {
            $file = $this->config_file;
        }
        $fp = @fopen($file, "r");
        if (!$fp) {
            return $this->raiseError($php_errormsg);
        }
        $contents = fread($fp, filesize($file));
        $data = unserialize($contents);
        if ($data === false) {
            return $this->raiseError("PEAR_Config::readConfigFile: bad data");
        }
        $this->configuration = $data;
        if ($defaults) {
            foreach ($data as $key => $value) {
                $this->defaulted[$key] = true;
            }
        }
    }

    // }}}
    // {{{ mergeConfigFile(file, [override], [defaults])

    function mergeConfigFile($file, $override = true, $defaults = false)
    {
        $fp = @fopen($file, "r");
        if (!$fp) {
            return $this->raiseError($php_errormsg);
        }
        $contents = fread($fp, filesize($file));
        $data = unserialize($contents);
        if ($data === false) {
            return $this->raiseError("PEAR_Config::mergeConfigFile: bad data");
        }
        foreach ($data as $key => $value) {
            if (isset($this->configuration[$key]) && !$override) {
                continue;
            }
            if ($defaults) {
                $this->defaulted[$key] = true;
            }
            $this->configuration[$key] = $value;
        }
    }

    // }}}
    // {{{ writeConfigFile([file], [what_keys])

    function writeConfigFile($file = null, $what_keys = 'userdefined')
    {
        print "storing $what_keys keys\n";
        if ($what_keys == 'both') {
            $this->writeConfigFile($file, 'userdefined');
            $this->writeConfigFile($file, 'default');
        }
        if ($file === null) {
            if ($what_keys == 'default') {
                $file = $this->defaults_file;
            } else {
                $file = $this->config_file;
            }
        }
        if ($what_keys == 'default') {
            $keys_to_store = array_intersect(array_keys($this->configuration),
                                             array_keys($this->defaulted));
        } elseif ($what_keys == 'all') {
            $keys_to_store = array_keys($this->configuration);
        } else { // user-defined keys
            $keys_to_store = array_diff(array_keys($this->configuration),
                                        array_keys($this->defaulted));
        }
        $data = array();
        foreach ($keys_to_store as $key) {
            $data[$key] = $this->configuration[$key];
        }
        $fp = @fopen($file, "w");
        if (!$fp) {
            return $this->raiseError("PEAR_Config::writeConfigFile fopen failed");
        }
        if (!@fwrite($fp, serialize($data))) {
            return $this->raiseError("PEAR_Config::writeConfigFile serialize failed");
        }
        return true;
    }

    // }}}
    // {{{ get(key)

    function get($key)
    {
        return @$this->configuration[$key];
    }

    // }}}
    // {{{ set(key, value, [default])

    function set($key, $value, $default = false)
    {
        $this->configuration[$key] = $value;
        if ($default) {
            $this->defaulted[$key] = true;
        } elseif (isset($this->defaulted[$key])) {
            unset($this->defaulted[$key]);
        }
    }

    // }}}
    // {{{ toDefault(key)

    function toDefault($key)
    {
        if (file_exists($this->defaults_file)) {
            // re-reads the defaults file each time, but hey it works
            unset($this->configuration[$key]);
            $this->mergeConfigFile($this->defaults_file, false, true);
        }
    }

    // }}}
    // {{{ isDefaulted(key)

    function isDefaulted($key)
    {
        return isset($this->defaulted[$key]);
    }

    // }}}
    // {{{ isDefined(key)

    function isDefined($key)
    {
        return isset($this->configuration[$key]);
    }

    // }}}
}

?>