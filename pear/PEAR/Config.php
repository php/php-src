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

require_once 'PEAR.php';

/**
 * Last created PEAR_Config instance.
 * @var object
 */
$GLOBALS['_PEAR_Config_last_instance'] = null;

define('PEAR_CONFIG_DEFAULT_DOCDIR',
       PHP_DATADIR.DIRECTORY_SEPARATOR.'pear'.DIRECTORY_SEPARATOR.'doc');
/**
 * This is a class for storing simple configuration values keeping
 * track of which are system-defined, user-defined or defaulted.  By
 * default, only user-defined settings are stored back to the user's
 * configuration file.
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

    /**
     * Array of config files used.
     *
     * @var array layer => config file
     */
    var $files = array(
        'system' => '',
        'user' => '',
        );

    var $layers = array();

    /**
     * Configuration data, two-dimensional array where the first
     * dimension is the config layer ('user', 'system' and 'default'),
     * and the second dimension is keyname => value.
     *
     * The order in the first dimension is important!  Earlier
     * layers will shadow later ones when a config value is
     * requested (if a 'user' value exists, it will be returned first,
     * then 'system' and finally 'default').
     *
     * @var array layer => array(keyname => value, ...)
     */
    var $configuration = array(
        'user' => array(),
        'system' => array(),
        'default' => array(),
        );

    /**
     * Information about the configuration data.  Stores the type,
     * default value and a documentation string for each configuration
     * value.
     *
     * @var array layer => array(infotype => value, ...)
     */
    var $configuration_info = array(
        'master_server' => array(
            'type' => 'string',
            'default' => 'pear.php.net',
            'doc' => 'name of the main PEAR server',
            ),
        'php_dir' => array(
            'type' => 'directory',
            'default' => PEAR_INSTALL_DIR,
            'doc' => 'directory where .php files are installed',
            ),
        'ext_dir' => array(
            'type' => 'directory',
            'default' => PEAR_EXTENSION_DIR,
            'doc' => 'directory where loadable extensions are installed',
            ),
        'doc_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_DOCDIR,
            'doc' => 'directory where documentation is located',
            ),
        'username' => array(
            'type' => 'string',
            'default' => '',
            'doc' => '(maintainers) your PEAR account name',
            ),
        'password' => array(
            'type' => 'password',
            'default' => '',
            'doc' => '(maintainers) your PEAR account password',
            ),
        'verbose' => array(
            'type' => 'integer',
            'default' => 1,
            'doc' => 'verbosity level',
            ),
        'testset1' => array(
            'type' => 'set',
            'default' => 'foo',
            'doc' => 'test set',
            'valid_set' => array('foo', 'bar'),
            ),
        );

    // }}}

    // {{{ PEAR_Config([file], [defaults_file])

    /**
     * Constructor.
     *
     * @param string (optional) file to read user-defined options from
     * @param string (optional) file to read system-wide defaults from
     *
     * @access public
     *
     * @see PEAR_Config::singleton
     */
    function PEAR_Config($user_file = '', $system_file = '')
    {
        $this->PEAR();
        $sl = DIRECTORY_SEPARATOR;
        if (empty($user_file)) {
            if (OS_WINDOWS) {
                $user_file = PHP_SYSCONFDIR . $sl . 'pear.ini';
            } else {
                $user_file = getenv('HOME') . $sl . '.pearrc';
            }
        }
        if (empty($system_file)) {
            if (OS_WINDOWS) {
                $system_file = PHP_SYSCONFDIR . $sl . 'pearsys.ini';
            } else {
                $system_file = PHP_SYSCONFDIR . $sl . 'pear.conf';
            }
        }
        $this->layers = array_keys($this->configuration);
        $this->files['user'] = $user_file;
        $this->files['system'] = $system_file;
        $GLOBALS['_PEAR_Config_last_instance'] = &$this;
        if ($user_file && file_exists($user_file)) {
            $this->readConfigFile($user_file);
        }
        if ($system_file && file_exists($system_file)) {
            $this->mergeConfigFile($system_file, false, true);
        }
        foreach ($this->configuration_info as $key => $info) {
            $this->configuration['default'][$key] = $info['default'];
        }
    }

    // }}}
    // {{{ singleton([file], [defaults_file])

    /**
     * Static singleton method.  If you want to keep only one instance
     * of this class in use, this method will give you a reference to
     * the last created PEAR_Config object if one exists, or create a
     * new object.
     *
     * @param string (optional) file to read user-defined options from
     * @param string (optional) file to read system-wide defaults from
     *
     * @return object an existing or new PEAR_Config instance
     *
     * @access public
     *
     * @see PEAR_Config::PEAR_Config
     */
    function &singleton($user_file = '', $system_file = '')
    {
        if (empty($GLOBALS['_PEAR_Config_last_instance'])) {
            $obj =& new PEAR_Config($user_file, $system_file);
            $GLOBALS['_PEAR_Config_last_instance'] = &$obj;
        }
        return $obj;
    }

    // }}}
    // {{{ readConfigFile([file], [defaults])

    /**
     * Reads configuration data from a file.  All existing values in
     * the config layer are discarded and replaced with data from the
     * file.
     *
     * @param string (optional) file to read from, if NULL or not
     * specified, the last-used file for the same layer (second param)
     * is used
     *
     * @param string (optional) config layer to insert data into
     * ('user' or 'system')
     *
     * @return bool TRUE on success or a PEAR error on failure
     *
     * @access public
     */
    function readConfigFile($file = null, $layer = 'user')
    {
        if (empty($this->files[$layer])) {
            return $this->raiseError("unknown config file type");
        }
        if ($file === null) {
            $file = $this->files[$layer];
        }
        $fp = @fopen($file, "r");
        if (!$fp) {
            return $this->raiseError("PEAR_Config::readConfigFile fopen('$file','r') failed");
        }
        $size = filesize($file);
        $contents = fread($fp, $size);
        $data = unserialize($contents);
        settype($data, 'array');
        if (empty($data) && $size > 1) {
            return $this->raiseError("PEAR_Config::readConfigFile: bad data");
        }
        $this->_decodeInput($data);
        $this->configuration[$layer] = $data;
        return true;
    }

    // }}}
    // {{{ mergeConfigFile(file, [override], [defaults])

    /**
     * Merges data into a config layer from a file.  Does the same
     * thing as readConfigFile, except it does not replace all
     * existing values in the config layer.
     *
     * @param string file to read from
     *
     * @param bool (optional) whether to overwrite existing data
     * (default TRUE)
     *
     * @param string config layer to insert data into ('user' or
     * 'system')
     *
     * @return bool TRUE on success or a PEAR error on failure
     *
     * @access public.
     */
    function mergeConfigFile($file, $override = true, $layer = 'user')
    {
        if (empty($this->files[$layer])) {
            return $this->raiseError("unknown config file type");
        }
        $fp = @fopen($file, "r");
        if (!$fp) {
            return $this->raiseError($php_errormsg);
        }
        $size = filesize($file);
        $contents = fread($fp, $size);
        $data = unserialize($contents);
        settype($data, 'array');
        if (empty($data) && $size > 1) {
            return $this->raiseError("PEAR_Config::mergeConfigFile: bad data");
        }
        $this->_decodeInput($data);
        if ($override) {
            $this->configuration[$layer] = array_merge($this->configuration[$layer], $data);
        } else {
            $this->configuration[$layer] = array_merge($data, $this->configuration[$layer]);
        }
        return true;
    }

    // }}}
    // {{{ writeConfigFile([file], [what_keys])

    /**
     * Writes data into a config layer from a file.
     *
     * @param string file to read from
     *
     * @param bool (optional) whether to overwrite existing data
     * (default TRUE)
     *
     * @param string config layer to insert data into ('user' or
     * 'system')
     *
     * @return bool TRUE on success or a PEAR error on failure
     *
     * @access public.
     */
    function writeConfigFile($file = null, $layer = 'user')
    {
        if ($layer == 'both' || $layer == 'all') {
            foreach ($this->files as $type => $file) {
                $err = $this->writeConfigFile($file, $type);
                if (PEAR::isError($err)) {
                    return $err;
                }
            }
            return true;
        }
        if (empty($this->files[$layer])) {
            return $this->raiseError("unknown config file type");
        }
        if ($file === null) {
            $file = $this->files[$layer];
        }
        $data = $this->configuration[$layer];
        var_dump($this->configuration);
        $this->_encodeOutput($data);
        if (!@is_writeable($file)) {
            return $this->raiseError("no write access to $file!");
        }
        $fp = @fopen($file, "w");
        if (!$fp) {
            return $this->raiseError("PEAR_Config::writeConfigFile fopen('$file','w') failed");
        }
        if (!@fwrite($fp, serialize($data))) {
            return $this->raiseError("PEAR_Config::writeConfigFile serialize failed");
        }
        return true;
    }

    // }}}
    // {{{ _encodeOutput(&data)

    /**
     * Encodes/scrambles configuration data before writing to files.
     * Currently, 'password' values will be base64-encoded as to avoid
     * that people spot cleartext passwords by accident.
     *
     * @param array (reference) array to encode values in
     *
     * @return bool TRUE on success
     *
     * @access private
     */
    function _encodeOutput(&$data)
    {
        foreach ($data as $key => $value) {
            if (!isset($this->configuration_info[$key])) {
                continue;
            }
            $type = $this->configuration_info[$key]['type'];
            switch ($type) {
                // we base64-encode passwords so they are at least
                // not shown in plain by accident
                case 'password': {
                    $data[$key] = base64_encode($data[$key]);
                    break;
                }
            }
        }
        return true;
    }

    // }}}
    // {{{ _decodeInput(&data)

    /**
     * Decodes/unscrambles configuration data after reading from files.
     *
     * @param array (reference) array to encode values in
     *
     * @return bool TRUE on success
     *
     * @access private
     *
     * @see PEAR_Config::_encodeOutput
     */
    function _decodeInput(&$data)
    {
        if (!is_array($data)) {
            return true;
        }
        foreach ($data as $key => $value) {
            if (!isset($this->configuration_info[$key])) {
                continue;
            }
            $type = $this->configuration_info[$key]['type'];
            switch ($type) {
                case 'password': {
                    $data[$key] = base64_decode($data[$key]);
                    break;
                }
            }
        }
        return true;
    }

    // }}}
    // {{{ get(key)

    /**
     * Returns a configuration value, prioritizing layers as per the
     * layers property.
     *
     * @param string config key
     *
     * @return mixed the config value, or NULL if not found
     *
     * @access public
     */
    function get($key)
    {
        foreach ($this->layers as $layer) {
            if (isset($this->configuration[$layer][$key])) {
                return $layer;
            }
        }
        return null;
    }

    // }}}
    // {{{ set(key, value, [default])

    /**
     * Set a config value in a specific layer (defaults to 'user').
     * Enforces the types defined in the configuration_info array.  An
     * integer config variable will be cast to int, and a set config
     * variable will be validated against its legal values.
     *
     * @param string config key
     *
     * @param string config value
     *
     * @param string (optional) config layer
     *
     * @return bool TRUE on success, FALSE on failure
     *
     * @access public
     */
    function set($key, $value, $layer = 'user')
    {
        if (empty($this->configuration[$layer]) || empty($this->configuration_info[$key])) {
            return false;
        }
        extract($this->configuration_info[$key], EXTR_OVERWRITE, 'k_');
        switch ($k_type) {
            case 'integer': {
                $value = (int)$value;
                break;
            }
            case 'set': {
                // If a valid_set is specified, require the value to
                // be in the set.  If there is no valid_set, accept
                // any value.
                if ($k_valid_set) {
                    reset($k_valid_set);
                    if ((key($k_valid_set) === 0 && !in_array($value, $k_valid_set)) ||
                        empty($k_valid_set[$value]))
                    {
                        return false;
                    }
                }
                break;
            }
        }
        $this->configuration[$layer][$key] = $value;
        return true;
    }

    // }}}
    // {{{ getType(key)

    /**
     * Get the type of a config value.
     *
     * @param string  config key
     *
     * @return string type, one of "string", "integer", "file",
     * "directory", "set" or "password".
     *
     * @access public
     *
     */
    function getType($key)
    {
        if (isset($this->configuration_info[$key])) {
            return $this->configuration_info[$key]['type'];
        }
        return false;
    }

    // }}}
    // {{{ getDocs(key)

    /**
     * Get the documentation for a config value.
     *
     * @param string  config key
     *
     * @return string documentation string
     *
     * @access public
     *
     */
    function getDocs($key)
    {
        if (isset($this->configuration_info[$key])) {
            return $this->configuration_info[$key]['doc'];
        }
        return false;
    }

    // }}}
    // {{{ getKeys()

    /**
     * Get all the current config keys.
     *
     * @return array simple array of config keys
     *
     * @access public
     */
    function getKeys()
    {
        $keys = array();
        foreach ($this->layers as $layer) {
            $keys = array_merge($keys, $this->configuration[$layer]);
        }
        return array_keys($keys);
    }

    // }}}
    // {{{ remove(key, [layer])

    /**
     * Unset the a config key in a specific config layer.
     *
     * @param string config key
     *
     * @param string (optional) config layer
     *
     * @return bool TRUE on success, FALSE on failure
     *
     * @access public
     */
    function remove($key, $layer = 'user')
    {
        if (isset($this->configuration[$layer][$key])) {
            unset($this->configuration[$layer][$key]);
            return true;
        }
        return false;
    }

    // }}}
    // {{{ toDefault(key)

    /**
     * Unset the user-defined value of a config key, reverting the
     * value to the system-defined one.
     *
     * @param string config key
     *
     * @return bool TRUE on success, FALSE on failure
     *
     * @access public
     */
    function toDefault($key)
    {
        trigger_error("PEAR_Config::toDefault() deprecated, use PEAR_Config::unset() instead", E_USER_NOTICE);
        return $this->remove($key, 'user');
    }

    // }}}
    // {{{ definedBy(key)

    /**
     * Tells what config layer that gets to define a key.
     *
     * @param string config key
     *
     * @return string the config layer, or an empty string if not found
     *
     * @access public
     */
    function definedBy($key)
    {
        foreach ($this->layers as $layer) {
            if (isset($this->configuration[$layer][$key])) {
                return $layer;
            }
        }
        return '';
    }

    // }}}
    // {{{ isDefaulted(key)

    /**
     * Tells whether a config value has a system-defined value.
     *
     * @param string   config key
     *
     * @return bool
     *
     * @access public
     *
     * @deprecated
     */
    function isDefaulted($key)
    {
        trigger_error("PEAR_Config::isDefaulted() deprecated, use PEAR_Config::definedBy() instead", E_USER_NOTICE);
        return $this->definedBy($key) == 'system';
    }

    // }}}
    // {{{ isDefined(key)

    /**
     * Tells whether a given key exists as a config value.
     *
     * @param string config key
     *
     * @return bool whether <config key> exists in this object
     *
     * @access public
     */
    function isDefined($key)
    {
        foreach ($this->layers as $layer) {
            if (isset($this->configuration[$layer][$key])) {
                return true;
            }
        }
        return false;
    }

    // }}}
}

?>