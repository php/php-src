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
// | Author: Stig Bakken <ssb@php.net>                                    |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';
require_once 'System.php';

/**
 * Last created PEAR_Config instance.
 * @var object
 */
$GLOBALS['_PEAR_Config_instance'] = null;

// Below we define constants with default values for all configuration
// parameters except username/password.  All of them can have their
// defaults set through environment variables.  The reason we use the
// PHP_ prefix is for some security, PHP protects environment
// variables starting with PHP_*.

if (getenv('PHP_PEAR_SYSCONF_DIR')) {
    define('PEAR_CONFIG_SYSCONFDIR', getenv('PHP_PEAR_SYSCONF_DIR'));
} elseif (getenv('SystemRoot')) {
    define('PEAR_CONFIG_SYSCONFDIR', getenv('SystemRoot'));
} else {
    define('PEAR_CONFIG_SYSCONFDIR', PHP_SYSCONFDIR);
}

// Default for master_server
if (getenv('PHP_PEAR_MASTER_SERVER')) {
    define('PEAR_CONFIG_DEFAULT_MASTER_SERVER', getenv('PHP_PEAR_MASTER_SERVER'));
} else {
    define('PEAR_CONFIG_DEFAULT_MASTER_SERVER', 'pear.php.net');
}

// Default for http_proxy
if (getenv('PHP_PEAR_HTTP_PROXY')) {
    define('PEAR_CONFIG_DEFAULT_HTTP_PROXY', getenv('PHP_PEAR_HTTP_PROXY'));
} elseif (getenv('http_proxy')) {
    define('PEAR_CONFIG_DEFAULT_HTTP_PROXY', getenv('http_proxy'));
} else {
    define('PEAR_CONFIG_DEFAULT_HTTP_PROXY', '');
}

// Default for php_dir
if (getenv('PHP_PEAR_INSTALL_DIR')) {
    define('PEAR_CONFIG_DEFAULT_PHP_DIR', getenv('PHP_PEAR_INSTALL_DIR'));
} else {
    if (@is_dir(PEAR_INSTALL_DIR.DIRECTORY_SEPARATOR.'lib')) {
        define('PEAR_CONFIG_DEFAULT_PHP_DIR',
               PEAR_INSTALL_DIR.DIRECTORY_SEPARATOR.'lib');
    } else {
        define('PEAR_CONFIG_DEFAULT_PHP_DIR', PEAR_INSTALL_DIR);
    }
}

// Default for ext_dir
if (getenv('PHP_PEAR_EXTENSION_DIR')) {
    define('PEAR_CONFIG_DEFAULT_EXT_DIR', getenv('PHP_PEAR_EXTENSION_DIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_EXT_DIR', ini_get('extension_dir'));
}

// Default for doc_dir
if (getenv('PHP_PEAR_DOC_DIR')) {
    define('PEAR_CONFIG_DEFAULT_DOC_DIR', getenv('PHP_PEAR_DOC_DIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_DOC_DIR',
           PEAR_INSTALL_DIR.DIRECTORY_SEPARATOR.'docs');
}

// Default for bin_dir
if (getenv('PHP_PEAR_BIN_DIR')) {
    define('PEAR_CONFIG_DEFAULT_BIN_DIR', getenv('PHP_PEAR_BIN_DIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_BIN_DIR', PHP_BINDIR);
}

// Default for data_dir
if (getenv('PHP_PEAR_DATA_DIR')) {
    define('PEAR_CONFIG_DEFAULT_DATA_DIR', getenv('PHP_PEAR_DATA_DIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_DATA_DIR',
           PEAR_INSTALL_DIR.DIRECTORY_SEPARATOR.'data');
}

// Default for test_dir
if (getenv('PHP_PEAR_TEST_DIR')) {
    define('PEAR_CONFIG_DEFAULT_TEST_DIR', getenv('PHP_PEAR_TEST_DIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_TEST_DIR',
           PEAR_INSTALL_DIR.DIRECTORY_SEPARATOR.'tests');
}

// Default for cache_dir
if (getenv('PHP_PEAR_CACHE_DIR')) {
    define('PEAR_CONFIG_DEFAULT_CACHE_DIR', getenv('PHP_PEAR_CACHE_DIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_CACHE_DIR',
           System::tmpdir() . DIRECTORY_SEPARATOR . 'pear' .
           DIRECTORY_SEPARATOR . 'cache');
}

// Default for php_bin
if (getenv('PHP_PEAR_PHP_BIN')) {
    define('PEAR_CONFIG_DEFAULT_PHP_BIN', getenv('PHP_PEAR_PHP_BIN'));
} else {
    define('PEAR_CONFIG_DEFAULT_PHP_BIN', PEAR_CONFIG_DEFAULT_BIN_DIR.
           DIRECTORY_SEPARATOR.'php'.(OS_WINDOWS ? '.exe' : ''));
}

// Default for verbose
if (getenv('PHP_PEAR_VERBOSE')) {
    define('PEAR_CONFIG_DEFAULT_VERBOSE', getenv('PHP_PEAR_VERBOSE'));
} else {
    define('PEAR_CONFIG_DEFAULT_VERBOSE', 1);
}

// Default for preferred_state
if (getenv('PHP_PEAR_PREFERRED_STATE')) {
    define('PEAR_CONFIG_DEFAULT_PREFERRED_STATE', getenv('PHP_PEAR_PREFERRED_STATE'));
} else {
    define('PEAR_CONFIG_DEFAULT_PREFERRED_STATE', 'stable');
}

// Default for umask
if (getenv('PHP_PEAR_UMASK')) {
    define('PEAR_CONFIG_DEFAULT_UMASK', getenv('PHP_PEAR_UMASK'));
} else {
    define('PEAR_CONFIG_DEFAULT_UMASK', decoct(umask()));
}

// Default for cache_ttl
if (getenv('PHP_PEAR_CACHE_TTL')) {
    define('PEAR_CONFIG_DEFAULT_CACHE_TTL', getenv('PHP_PEAR_CACHE_TTL'));
} else {
    define('PEAR_CONFIG_DEFAULT_CACHE_TTL', 3600);
}

// Default for sig_type
if (getenv('PHP_PEAR_SIG_TYPE')) {
    define('PEAR_CONFIG_DEFAULT_SIG_TYPE', getenv('PHP_PEAR_SIG_TYPE'));
} else {
    define('PEAR_CONFIG_DEFAULT_SIG_TYPE', 'gpg');
}

// Default for sig_bin
if (getenv('PHP_PEAR_SIG_BIN')) {
    define('PEAR_CONFIG_DEFAULT_SIG_BIN', getenv('PHP_PEAR_SIG_BIN'));
} else {
    define('PEAR_CONFIG_DEFAULT_SIG_BIN',
           System::which(
               'gpg', OS_WINDOWS ? 'c:\gnupg\gpg.exe' : '/usr/local/bin/gpg'));
}

// Default for sig_keydir
if (getenv('PHP_PEAR_SIG_KEYDIR')) {
    define('PEAR_CONFIG_DEFAULT_SIG_KEYDIR', getenv('PHP_PEAR_SIG_KEYDIR'));
} else {
    define('PEAR_CONFIG_DEFAULT_SIG_KEYDIR',
           PEAR_CONFIG_SYSCONFDIR . DIRECTORY_SEPARATOR . 'pearkeys');
}

/**
 * This is a class for storing configuration data, keeping track of
 * which are system-defined, user-defined or defaulted.
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
        // Internet Access
        'master_server' => array(
            'type' => 'string',
            'default' => 'pear.php.net',
            'doc' => 'name of the main PEAR server',
            'prompt' => 'PEAR server',
            'group' => 'Internet Access',
            ),
        'http_proxy' => array(
            'type' => 'string',
            'default' => PEAR_CONFIG_DEFAULT_HTTP_PROXY,
            'doc' => 'HTTP proxy (host:port) to use when downloading packages',
            'prompt' => 'HTTP Proxy Server Address',
            'group' => 'Internet Access',
            ),
        // File Locations
        'php_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_PHP_DIR,
            'doc' => 'directory where .php files are installed',
            'prompt' => 'PEAR directory',
            'group' => 'File Locations',
            ),
        'ext_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_EXT_DIR,
            'doc' => 'directory where loadable extensions are installed',
            'prompt' => 'PHP extension directory',
            'group' => 'File Locations',
            ),
        'doc_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_DOC_DIR,
            'doc' => 'directory where documentation is installed',
            'prompt' => 'PEAR documentation directory',
            'group' => 'File Locations',
            ),
        'bin_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_BIN_DIR,
            'doc' => 'directory where executables are installed',
            'prompt' => 'PEAR executables directory',
            'group' => 'File Locations',
            ),
        'data_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_DATA_DIR,
            'doc' => 'directory where data files are installed',
            'prompt' => 'PEAR data directory',
            'group' => 'File Locations (Advanced)',
            ),
        'test_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_TEST_DIR,
            'doc' => 'directory where regression tests are installed',
            'prompt' => 'PEAR test directory',
            'group' => 'File Locations (Advanced)',
            ),
        'cache_dir' => array(
            'type' => 'directory',
            'default' => PEAR_CONFIG_DEFAULT_CACHE_DIR,
            'doc' => 'directory which is used for XMLRPC cache',
            'prompt' => 'PEAR Installer cache directory',
            'group' => 'File Locations (Advanced)',
            ),
        'php_bin' => array(
            'type' => 'file',
            'default' => PEAR_CONFIG_DEFAULT_PHP_BIN,
            'doc' => 'PHP CLI/CGI binary for executing scripts',
            'prompt' => 'PHP CLI/CGI binary',
            'group' => 'File Locations (Advanced)',
            ),
        // Maintainers
        'username' => array(
            'type' => 'string',
            'default' => '',
            'doc' => '(maintainers) your PEAR account name',
            'prompt' => 'PEAR username (for maintainers)',
            'group' => 'Maintainers',
            ),
        'password' => array(
            'type' => 'password',
            'default' => '',
            'doc' => '(maintainers) your PEAR account password',
            'prompt' => 'PEAR password (for maintainers)',
            'group' => 'Maintainers',
            ),
        // Advanced
        'verbose' => array(
            'type' => 'integer',
            'default' => PEAR_CONFIG_DEFAULT_VERBOSE,
            'doc' => 'verbosity level
0: really quiet
1: somewhat quiet
2: verbose
3: debug',
            'prompt' => 'Debug Log Level',
            'group' => 'Advanced',
            ),
        'preferred_state' => array(
            'type' => 'set',
            'default' => PEAR_CONFIG_DEFAULT_PREFERRED_STATE,
            'doc' => 'the installer will prefer releases with this state when installing packages without a version or state specified',
            'valid_set' => array(
                'stable', 'beta', 'alpha', 'devel', 'snapshot'),
            'prompt' => 'Preferred Package State',
            'group' => 'Advanced',
            ),
        'umask' => array(
            'type' => 'mask',
            'default' => PEAR_CONFIG_DEFAULT_UMASK,
            'doc' => 'umask used when creating files (Unix-like systems only)',
            'prompt' => 'Unix file mask',
            'group' => 'Advanced',
            ),
        'cache_ttl' => array(
            'type' => 'integer',
            'default' => PEAR_CONFIG_DEFAULT_CACHE_TTL,
            'doc' => 'amount of secs where the local cache is used and not updated',
            'prompt' => 'Cache TimeToLive',
            'group' => 'Advanced',
            ),
        'sig_type' => array(
            'type' => 'set',
            'default' => PEAR_CONFIG_DEFAULT_SIG_TYPE,
            'doc' => 'which package signature mechanism to use',
            'valid_set' => array('gpg'),
            'prompt' => 'Package Signature Type',
            'group' => 'Maintainers',
            ),
        'sig_bin' => array(
            'type' => 'string',
            'default' => PEAR_CONFIG_DEFAULT_SIG_BIN,
            'doc' => 'which package signature mechanism to use',
            'prompt' => 'Signature Handling Program',
            'group' => 'Maintainers',
            ),
        'sig_keyid' => array(
            'type' => 'string',
            'default' => '',
            'doc' => 'which key to use for signing with',
            'prompt' => 'Signature Key Id',
            'group' => 'Maintainers',
            ),
        'sig_keydir' => array(
            'type' => 'string',
            'default' => PEAR_CONFIG_DEFAULT_SIG_KEYDIR,
            'doc' => 'which package signature mechanism to use',
            'prompt' => 'Signature Key Directory',
            'group' => 'Maintainers',
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
                $user_file = PEAR_CONFIG_SYSCONFDIR . $sl . 'pear.ini';
            } else {
                $user_file = getenv('HOME') . $sl . '.pearrc';
            }
        }
        if (empty($system_file)) {
            if (OS_WINDOWS) {
                $system_file = PEAR_CONFIG_SYSCONFDIR . $sl . 'pearsys.ini';
            } else {
                $system_file = PEAR_CONFIG_SYSCONFDIR . $sl . 'pear.conf';
            }
        }
        $this->layers = array_keys($this->configuration);
        $this->files['user'] = $user_file;
        $this->files['system'] = $system_file;
        if ($user_file && file_exists($user_file)) {
            $this->readConfigFile($user_file);
        }
        if ($system_file && file_exists($system_file)) {
            $this->mergeConfigFile($system_file, false, 'system');
        }
        foreach ($this->configuration_info as $key => $info) {
            $this->configuration['default'][$key] = $info['default'];
        }
        //$GLOBALS['_PEAR_Config_instance'] = &$this;
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
        if (is_object($GLOBALS['_PEAR_Config_instance'])) {
            return $GLOBALS['_PEAR_Config_instance'];
        }
        $GLOBALS['_PEAR_Config_instance'] =
             &new PEAR_Config($user_file, $system_file);
        return $GLOBALS['_PEAR_Config_instance'];
    }

    // }}}
    // {{{ readConfigFile([file], [layer])

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
            return $this->raiseError("unknown config file type `$layer'");
        }
        if ($file === null) {
            $file = $this->files[$layer];
        }
        $data = $this->_readConfigDataFrom($file);
        if (PEAR::isError($data)) {
            return $data;
        }
        $this->_decodeInput($data);
        $this->configuration[$layer] = $data;
        return true;
    }

    // }}}
    // {{{ mergeConfigFile(file, [override], [layer])

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
            return $this->raiseError("unknown config file type `$layer'");
        }
        if ($file === null) {
            $file = $this->files[$layer];
        }
        $data = $this->_readConfigDataFrom($file);
        if (PEAR::isError($data)) {
            return $data;
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
    // {{{ writeConfigFile([file], [layer])

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
            return $this->raiseError("unknown config file type `$layer'");
        }
        if ($file === null) {
            $file = $this->files[$layer];
        }
        $data = $this->configuration[$layer];
        $this->_encodeOutput($data);
        if (!@System::mkDir("-p " . dirname($file))) {
            return $this->raiseError("could not create directory: " . dirname($file));
        }
        if (@is_file($file) && !@is_writeable($file)) {
            return $this->raiseError("no write access to $file!");
        }
        $fp = @fopen($file, "w");
        if (!$fp) {
            return $this->raiseError("PEAR_Config::writeConfigFile fopen('$file','w') failed");
        }
        $contents = "#PEAR_Config 0.9\n" . serialize($data);
        if (!@fwrite($fp, $contents)) {
            return $this->raiseError("PEAR_Config::writeConfigFile: fwrite failed");
        }
        return true;
    }

    // }}}
    // {{{ _readConfigDataFrom(file)

    /**
     * Reads configuration data from a file and returns the parsed data
     * in an array.
     *
     * @param string file to read from
     *
     * @return array configuration data or a PEAR error on failure
     *
     * @access private
     */
    function _readConfigDataFrom($file)
    {
        $fp = @fopen($file, "r");
        if (!$fp) {
            return $this->raiseError("PEAR_Config::readConfigFile fopen('$file','r') failed");
        }
        $size = filesize($file);
        $contents = fread($fp, $size);
        fclose($fp);
        $version = '0.1';
        if (preg_match('/^#PEAR_Config\s+(\S+)\s+/si', $contents, $matches)) {
            $version = $matches[1];
            $contents = substr($contents, strlen($matches[0]));
        }
        if (version_compare("$version", '1', '<')) {
            $data = unserialize($contents);
            if (!is_array($data)) {
                if (strlen(trim($contents)) > 0) {
                    $error = "PEAR_Config: bad data in $file";
//                if (isset($this)) {
                    return $this->raiseError($error);
//                } else {
//                    return PEAR::raiseError($error);
                } else {
                    $data = array();
                }
            }
        // add parsing of newer formats here...
        } else {
            return $this->raiseError("$file: unknown version `$version'");
        }
        return $data;
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
                case 'mask': {
                    $data[$key] = octdec($data[$key]);
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
                case 'mask': {
                    $data[$key] = decoct($data[$key]);
                    break;
                }
            }
        }
        return true;
    }

    // }}}
    // {{{ get(key, [layer])

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
    function get($key, $layer = null)
    {
        if ($layer === null) {
            foreach ($this->layers as $layer) {
                if (isset($this->configuration[$layer][$key])) {
                    return $this->configuration[$layer][$key];
                }
            }
        } elseif (isset($this->configuration[$layer][$key])) {
            return $this->configuration[$layer][$key];
        }
        return null;
    }

    // }}}
    // {{{ set(key, value, [layer])

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
        if (empty($this->configuration_info[$key])) {
            return false;
        }
        extract($this->configuration_info[$key]);
        switch ($type) {
            case 'integer':
                $value = (int)$value;
                break;
            case 'set': {
                // If a valid_set is specified, require the value to
                // be in the set.  If there is no valid_set, accept
                // any value.
                if ($valid_set) {
                    reset($valid_set);
                    if ((key($valid_set) === 0 && !in_array($value, $valid_set)) ||
                        (key($valid_set) !== 0 && empty($valid_set[$value])))
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
    // {{{ getPrompt(key)

    /**
     * Get the short documentation for a config value.
     *
     * @param string  config key
     *
     * @return string short documentation string
     *
     * @access public
     *
     */
    function getPrompt($key)
    {
        if (isset($this->configuration_info[$key])) {
            return $this->configuration_info[$key]['prompt'];
        }
        return false;
    }
    // }}}
    // {{{ getGroup(key)

    /**
     * Get the parameter group for a config key.
     *
     * @param string  config key
     *
     * @return string parameter group
     *
     * @access public
     *
     */
    function getGroup($key)
    {
        if (isset($this->configuration_info[$key])) {
            return $this->configuration_info[$key]['group'];
        }
        return false;
    }

    // }}}
    // {{{ getGroups()

    /**
     * Get the list of parameter groups.
     *
     * @return array list of parameter groups
     *
     * @access public
     *
     */
    function getGroups()
    {
        $tmp = array();
        foreach ($this->configuration_info as $key => $info) {
            $tmp[$info['group']] = 1;
        }
        return array_keys($tmp);
    }

    // }}}
    // {{{ getGroupKeys()

    /**
     * Get the list of the parameters in a group.
     *
     * @param string $group parameter group
     *
     * @return array list of parameters in $group
     *
     * @access public
     *
     */
    function getGroupKeys($group)
    {
        $keys = array();
        foreach ($this->configuration_info as $key => $info) {
            if ($info['group'] == $group) {
                $keys[] = $key;
            }
        }
        return $keys;
    }

    // }}}
    // {{{ getSetValues(key)

    /**
     * Get the list of allowed set values for a config value.  Returns
     * NULL for config values that are not sets.
     *
     * @param string  config key
     *
     * @return array enumerated array of set values, or NULL if the
     *               config key is unknown or not a set
     *
     * @access public
     *
     */
    function getSetValues($key)
    {
        if (isset($this->configuration_info[$key]) &&
            isset($this->configuration_info[$key]['type']) &&
            $this->configuration_info[$key]['type'] == 'set')
        {
            $valid_set = $this->configuration_info[$key]['valid_set'];
            reset($valid_set);
            if (key($valid_set) === 0) {
                return $valid_set;
            }
            return array_keys($valid_set);
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
     * Remove the a config key from a specific config layer.
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
    // {{{ removeLayer(layer)

    /**
     * Temporarily remove an entire config layer.  USE WITH CARE!
     *
     * @param string config key
     *
     * @param string (optional) config layer
     *
     * @return bool TRUE on success, FALSE on failure
     *
     * @access public
     */
    function removeLayer($layer)
    {
        if (isset($this->configuration[$layer])) {
            unset($this->configuration[$layer]);
            return true;
        }
        return false;
    }

    // }}}
    // {{{ store([layer])

    /**
     * Stores configuration data in a layer.
     *
     * @param string config layer to store
     *
     * @return bool TRUE on success, or PEAR error on failure
     *
     * @access public
     */
    function store($layer = 'user')
    {
        return $this->writeConfigFile(null, $layer);
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
        trigger_error("PEAR_Config::toDefault() deprecated, use PEAR_Config::remove() instead", E_USER_NOTICE);
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
    // {{{ isDefinedLayer(key)

    /**
     * Tells whether a given config layer exists.
     *
     * @param string config layer
     *
     * @return bool whether <config layer> exists in this object
     *
     * @access public
     */
    function isDefinedLayer($layer)
    {
        return isset($this->configuration[$layer]);
    }

    // }}}
    // {{{ getLayers()

    /**
     * Returns the layers defined (except the 'default' one)
     *
     * @return array of the defined layers
     */
    function getLayers()
    {
        $cf = $this->configuration;
        unset($cf['default']);
        return array_keys($cf);
    }

    // }}}
}

?>
