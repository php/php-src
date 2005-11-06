<?php
/**
 * PEAR_DependencyDB, advanced installed packages dependency database
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Tomas V. V. Cox <cox@idecnet.com>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */

/**
 * Needed for error handling
 */
require_once 'PEAR.php';
require_once 'PEAR/Config.php';

/**
 * Track dependency relationships between installed packages
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @author     Tomas V.V.Cox <cox@idec.net.com>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_DependencyDB
{
    // {{{ properties

    /**
     * This is initialized by {@link setConfig()}
     * @var PEAR_Config
     * @access private
     */
    var $_config;
    /**
     * This is initialized by {@link setConfig()}
     * @var PEAR_Registry
     * @access private
     */
    var $_registry;
    /**
     * Filename of the dependency DB (usually .depdb)
     * @var string
     * @access private
     */
    var $_depdb = false;
    /**
     * File name of the lockfile (usually .depdblock)
     * @var string
     * @access private
     */
    var $_lockfile = false;
    /**
     * Open file resource for locking the lockfile
     * @var resource|false
     * @access private
     */
    var $_lockFp = false;
    /**
     * API version of this class, used to validate a file on-disk
     * @var string
     * @access private
     */
    var $_version = '1.0';
    /**
     * Cached dependency database file
     * @var array|null
     * @access private
     */
    var $_cache;

    // }}}
    // {{{ & singleton()

    /**
     * Get a raw dependency database.  Calls setConfig() and assertDepsDB()
     * @param PEAR_Config
     * @param string|false full path to the dependency database, or false to use default
     * @return PEAR_DependencyDB|PEAR_Error
     * @static
     */
    function &singleton(&$config, $depdb = false)
    {
        if (!isset($GLOBALS['_PEAR_DEPENDENCYDB_INSTANCE']
              [$config->get('php_dir', null, 'pear.php.net')])) {
            $a = new PEAR_DependencyDB;
            $GLOBALS['_PEAR_DEPENDENCYDB_INSTANCE']
              [$config->get('php_dir', null, 'pear.php.net')] = &$a;
            $a->setConfig($config, $depdb);
            if (PEAR::isError($e = $a->assertDepsDB())) {
                return $e;
            }
        }
        return $GLOBALS['_PEAR_DEPENDENCYDB_INSTANCE']
              [$config->get('php_dir', null, 'pear.php.net')];
    }

    /**
     * Set up the registry/location of dependency DB
     * @param PEAR_Config|false
     * @param string|false full path to the dependency database, or false to use default
     */
    function setConfig(&$config, $depdb = false)
    {
        if (!$config) {
            $this->_config = &PEAR_Config::singleton();
        } else {
            $this->_config = &$config;
        }
        $this->_registry = &$this->_config->getRegistry();
        if (!$depdb) {
            $this->_depdb = $this->_config->get('php_dir', null, 'pear.php.net') .
                DIRECTORY_SEPARATOR . '.depdb';
        } else {
            $this->_depdb = $depdb;
        }
        $this->_lockfile = dirname($this->_depdb) . DIRECTORY_SEPARATOR . '.depdblock';
    }
    // }}}

    function hasWriteAccess()
    {
        if (!@file_exists($this->_depdb)) {
            $dir = $this->_depdb;
            while ($dir && $dir != '.') {
                $dir = dirname($dir); // cd ..
                if ($dir != '.' && @file_exists($dir)) {
                    if (@is_writeable($dir)) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
            return false;
        }
        return @is_writeable($this->_depdb);
    }

    // {{{ assertDepsDB()

    /**
     * Create the dependency database, if it doesn't exist.  Error if the database is
     * newer than the code reading it.
     * @return void|PEAR_Error
     */
    function assertDepsDB()
    {
        if (!is_file($this->_depdb)) {
            $this->rebuildDB();
        } else {
            $depdb = $this->_getDepDB();
            // Datatype format has been changed, rebuild the Deps DB
            if ($depdb['_version'] < $this->_version) {
                $this->rebuildDB();
            }
            if ($depdb['_version']{0} > $this->_version{0}) {
                return PEAR::raiseError('Dependency database is version ' .
                    $depdb['_version'] . ', and we are version ' .
                    $this->_version . ', cannot continue');
            }
        }
    }

    /**
     * Get a list of installed packages that depend on this package
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2|array
     * @return array|false
     */
    function getDependentPackages(&$pkg)
    {
        $data = $this->_getDepDB();
        if (is_object($pkg)) {
            $channel = strtolower($pkg->getChannel());
            $package = strtolower($pkg->getPackage());
        } else {
            $channel = strtolower($pkg['channel']);
            $package = strtolower($pkg['package']);
        }
        if (isset($data['packages'][$channel][$package])) {
            return $data['packages'][$channel][$package];
        }
        return false;
    }

    /**
     * Get a list of the actual dependencies of installed packages that depend on
     * a package.
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2|array
     * @return array|false
     */
    function getDependentPackageDependencies(&$pkg)
    {
        $data = $this->_getDepDB();
        if (is_object($pkg)) {
            $channel = strtolower($pkg->getChannel());
            $package = strtolower($pkg->getPackage());
        } else {
            $channel = strtolower($pkg['channel']);
            $package = strtolower($pkg['package']);
        }
        $depend = $this->getDependentPackages($pkg);
        if (!$depend) {
            return false;
        }
        $dependencies = array();
        foreach ($depend as $info) {
            $temp = $this->getDependencies($info);
            foreach ($temp as $dep) {
                if (strtolower($dep['dep']['channel']) == strtolower($channel) &&
                      strtolower($dep['dep']['name']) == strtolower($package)) {
                    $dependencies[$info['channel']][$info['package']][] = $dep;
                }
            }
        }
        return $dependencies;
    }

    /**
     * Get a list of dependencies of this installed package
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2|array
     * @return array|false
     */
    function getDependencies(&$pkg)
    {
        if (is_object($pkg)) {
            $channel = strtolower($pkg->getChannel());
            $package = strtolower($pkg->getPackage());
        } else {
            $channel = strtolower($pkg['channel']);
            $package = strtolower($pkg['package']);
        }
        $data = $this->_getDepDB();
        if (isset($data['dependencies'][$channel][$package])) {
            return $data['dependencies'][$channel][$package];
        }
        return false;
    }

    /**
     * Determine whether $parent depends on $child, near or deep
     * @param array|PEAR_PackageFile_v2|PEAR_PackageFile_v2
     * @param array|PEAR_PackageFile_v2|PEAR_PackageFile_v2
     */
    function dependsOn($parent, $child)
    {
        $c = array();
        $this->_getDepDB();
        return $this->_dependsOn($parent, $child, $c);
    }
    
    function _dependsOn($parent, $child, &$checked)
    {
        if (is_object($parent)) {
            $channel = strtolower($parent->getChannel());
            $package = strtolower($parent->getPackage());
        } else {
            $channel = strtolower($parent['channel']);
            $package = strtolower($parent['package']);
        }
        if (is_object($child)) {
            $depchannel = strtolower($child->getChannel());
            $deppackage = strtolower($child->getPackage());
        } else {
            $depchannel = strtolower($child['channel']);
            $deppackage = strtolower($child['package']);
        }
        if (isset($checked[$channel][$package][$depchannel][$deppackage])) {
            return false; // avoid endless recursion
        }
        $checked[$channel][$package][$depchannel][$deppackage] = true;
        if (!isset($this->_cache['dependencies'][$channel][$package])) {
            return false;
        }
        foreach ($this->_cache['dependencies'][$channel][$package] as $info) {
            if (strtolower($info['dep']['channel']) == strtolower($depchannel) &&
                  strtolower($info['dep']['name']) == strtolower($deppackage)) {
                return true;
            }
        }
        foreach ($this->_cache['dependencies'][$channel][$package] as $info) {
            if ($this->_dependsOn(array(
                    'channel' => $info['dep']['channel'],
                    'package' => $info['dep']['name']), $child, $checked)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Register dependencies of a package that is being installed or upgraded
     * @param PEAR_PackageFile_v2|PEAR_PackageFile_v2
     */
    function installPackage(&$package)
    {
        $data = $this->_getDepDB();
        unset($this->_cache);
        $this->_setPackageDeps($data, $package);
        $this->_writeDepDB($data);
    }

    /**
     * Remove dependencies of a package that is being uninstalled, or upgraded.
     *
     * Upgraded packages first uninstall, then install
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2|array If an array, then it must have
     *        indices 'channel' and 'package'
     */
    function uninstallPackage(&$pkg)
    {
        $data = $this->_getDepDB();
        unset($this->_cache);
        if (is_object($pkg)) {
            $channel = strtolower($pkg->getChannel());
            $package = strtolower($pkg->getPackage());
        } else {
            $channel = strtolower($pkg['channel']);
            $package = strtolower($pkg['package']);
        }
        if (!isset($data['dependencies'][$channel][$package])) {
            return true;
        }
        foreach ($data['dependencies'][$channel][$package] as $dep) {
            $found = false;
            if (isset($dep['dep']['uri'])) {
                $depchannel = '__uri';
            } else {
                $depchannel = strtolower($dep['dep']['channel']);
            }
            if (isset($data['packages'][$depchannel][strtolower($dep['dep']['name'])])) {
                foreach ($data['packages'][$depchannel][strtolower($dep['dep']['name'])] as
                      $i => $info) {
                    if ($info['channel'] == $channel &&
                          $info['package'] == $package) {
                        $found = true;
                        break;
                    }
                }
            }
            if ($found) {
                unset($data['packages'][$depchannel][strtolower($dep['dep']['name'])][$i]);
                if (!count($data['packages'][$depchannel][strtolower($dep['dep']['name'])])) {
                    unset($data['packages'][$depchannel][strtolower($dep['dep']['name'])]);
                    if (!count($data['packages'][$depchannel])) {
                        unset($data['packages'][$depchannel]);
                    }
                } else {
                    $data['packages'][$depchannel][strtolower($dep['dep']['name'])] =
                        array_values(
                            $data['packages'][$depchannel][strtolower($dep['dep']['name'])]);
                }
            }
        }
        unset($data['dependencies'][$channel][$package]);
        if (!count($data['dependencies'][$channel])) {
            unset($data['dependencies'][$channel]);
        }
        if (!count($data['dependencies'])) {
            unset($data['dependencies']);
        }
        if (!count($data['packages'])) {
            unset($data['packages']);
        }
        $this->_writeDepDB($data);
    }

    /**
     * Rebuild the dependency DB by reading registry entries.
     * @return true|PEAR_Error
     */
    function rebuildDB()
    {
        $depdb = array('_version' => $this->_version);
        if (!$this->hasWriteAccess()) {
            // allow startup for read-only with older Registry
            return $depdb;
        }
        $packages = $this->_registry->listAllPackages();
        foreach ($packages as $channel => $ps) {
            foreach ($ps as $package) {
                $package = $this->_registry->getPackage($package, $channel);
                $this->_setPackageDeps($depdb, $package);
            }
        }
        $error = $this->_writeDepDB($depdb);
        if (PEAR::isError($error)) {
            return $error;
        }
        $this->_cache = $depdb;
        return true;
    }

    /**
     * Register usage of the dependency DB to prevent race conditions
     * @param int one of the LOCK_* constants
     * @return true|PEAR_Error
     * @access private
     */
    function _lock($mode = LOCK_EX)
    {
        if (!eregi('Windows 9', php_uname())) {
            if ($mode != LOCK_UN && is_resource($this->_lockFp)) {
                // XXX does not check type of lock (LOCK_SH/LOCK_EX)
                return true;
            }
            $open_mode = 'w';
            // XXX People reported problems with LOCK_SH and 'w'
            if ($mode === LOCK_SH) {
                if (@!is_file($this->_lockfile)) {
                    touch($this->_lockfile);
                }
                $open_mode = 'r';
            }

            if (!is_resource($this->_lockFp)) {
                $this->_lockFp = @fopen($this->_lockfile, $open_mode);
            }
            if (!is_resource($this->_lockFp)) {
                return PEAR::raiseError("could not create Dependency lock file" .
                                         (isset($php_errormsg) ? ": " . $php_errormsg : ""));
            }
            if (!(int)flock($this->_lockFp, $mode)) {
                switch ($mode) {
                    case LOCK_SH: $str = 'shared';    break;
                    case LOCK_EX: $str = 'exclusive'; break;
                    case LOCK_UN: $str = 'unlock';    break;
                    default:      $str = 'unknown';   break;
                }
                return PEAR::raiseError("could not acquire $str lock ($this->_lockfile)");
            }
        }
        return true;
    }

    /**
     * Release usage of dependency DB
     * @return true|PEAR_Error
     * @access private
     */
    function _unlock()
    {
        $ret = $this->_lock(LOCK_UN);
        if (is_resource($this->_lockFp)) {
            fclose($this->_lockFp);
        }
        $this->_lockFp = null;
        return $ret;
    }

    /**
     * Load the dependency database from disk, or return the cache
     * @return array|PEAR_Error
     */
    function _getDepDB()
    {
        if (!$this->hasWriteAccess()) {
            return array('_version' => $this->_version);
        }
        if (isset($this->_cache)) {
            return $this->_cache;
        }
        if (!$fp = fopen($this->_depdb, 'r')) {
            $err = PEAR::raiseError("Could not open dependencies file `".$this->_depdb."'");
            return $err;
        }
        $rt = get_magic_quotes_runtime();
        set_magic_quotes_runtime(0);
        clearstatcache();
        if (function_exists('file_get_contents')) {
            fclose($fp);
            $data = unserialize(file_get_contents($this->_depdb));
        } else {
            $data = unserialize(fread($fp, filesize($this->_depdb)));
            fclose($fp);
        }
        set_magic_quotes_runtime($rt);
        $this->_cache = $data;
        return $data;
    }

    /**
     * Write out the dependency database to disk
     * @param array the database
     * @return true|PEAR_Error
     * @access private
     */
    function _writeDepDB(&$deps)
    {
        if (PEAR::isError($e = $this->_lock(LOCK_EX))) {
            return $e;
        }
        if (!$fp = fopen($this->_depdb, 'wb')) {
            $this->_unlock();
            return PEAR::raiseError("Could not open dependencies file `".$this->_depdb."' for writing");
        }
        $rt = get_magic_quotes_runtime();
        set_magic_quotes_runtime(0);
        fwrite($fp, serialize($deps));
        set_magic_quotes_runtime($rt);
        fclose($fp);
        $this->_unlock();
        $this->_cache = $deps;
        return true;
    }

    /**
     * Register all dependencies from a package in the dependencies database, in essence
     * "installing" the package's dependency information
     * @param array the database
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @access private
     */
    function _setPackageDeps(&$data, &$pkg)
    {
        $pkg->setConfig($this->_config);
        if ($pkg->getPackagexmlVersion() == '1.0') {
            $gen = &$pkg->getDefaultGenerator();
            $deps = $gen->dependenciesToV2();
        } else {
            $deps = $pkg->getDeps(true);
        }
        if (!$deps) {
            return;
        }
        $data['dependencies'][strtolower($pkg->getChannel())][strtolower($pkg->getPackage())]
            = array();
        if (isset($deps['required']['package'])) {
            if (!isset($deps['required']['package'][0])) {
                $deps['required']['package'] = array($deps['required']['package']);
            }
            foreach ($deps['required']['package'] as $dep) {
                $this->_registerDep($data, $pkg, $dep, 'required');
            }
        }
        if (isset($deps['optional']['package'])) {
            if (!isset($deps['optional']['package'][0])) {
                $deps['optional']['package'] = array($deps['optional']['package']);
            }
            foreach ($deps['optional']['package'] as $dep) {
                $this->_registerDep($data, $pkg, $dep, 'optional');
            }
        }
        if (isset($deps['required']['subpackage'])) {
            if (!isset($deps['required']['subpackage'][0])) {
                $deps['required']['subpackage'] = array($deps['required']['subpackage']);
            }
            foreach ($deps['required']['subpackage'] as $dep) {
                $this->_registerDep($data, $pkg, $dep, 'required');
            }
        }
        if (isset($deps['optional']['subpackage'])) {
            if (!isset($deps['optional']['subpackage'][0])) {
                $deps['optional']['subpackage'] = array($deps['optional']['subpackage']);
            }
            foreach ($deps['optional']['subpackage'] as $dep) {
                $this->_registerDep($data, $pkg, $dep, 'optional');
            }
        }
        if (isset($deps['group'])) {
            if (!isset($deps['group'][0])) {
                $deps['group'] = array($deps['group']);
            }
            foreach ($deps['group'] as $group) {
                if (isset($group['package'])) {
                    if (!isset($group['package'][0])) {
                        $group['package'] = array($group['package']);
                    }
                    foreach ($group['package'] as $dep) {
                        $this->_registerDep($data, $pkg, $dep, 'optional',
                            $group['attribs']['name']);
                    }
                }
                if (isset($group['subpackage'])) {
                    if (!isset($group['subpackage'][0])) {
                        $group['subpackage'] = array($group['subpackage']);
                    }
                    foreach ($group['subpackage'] as $dep) {
                        $this->_registerDep($data, $pkg, $dep, 'optional',
                            $group['attribs']['name']);
                    }
                }
            }
        }
        if ($data['dependencies'][strtolower($pkg->getChannel())]
              [strtolower($pkg->getPackage())] == array()) {
            unset($data['dependencies'][strtolower($pkg->getChannel())]
              [strtolower($pkg->getPackage())]);
            if (!count($data['dependencies'][strtolower($pkg->getChannel())])) {
                unset($data['dependencies'][strtolower($pkg->getChannel())]);
            }
        }
    }

    /**
     * @param array the database
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @param array the specific dependency
     * @param required|optional whether this is a required or an optional dep
     * @param string|false dependency group this dependency is from, or false for ordinary dep
     */
    function _registerDep(&$data, &$pkg, $dep, $type, $group = false)
    {
        $info = array(
            'dep' => $dep,
            'type' => $type,
            'group' => $group);

        if (isset($dep['channel'])) {
            $depchannel = $dep['channel'];
        } else {
            $depchannel = '__uri';
        }
        $data['dependencies'][strtolower($pkg->getChannel())][strtolower($pkg->getPackage())][]
            = $info;
        if (isset($data['packages'][strtolower($depchannel)][strtolower($dep['name'])])) {
            $found = false;
            foreach ($data['packages'][strtolower($depchannel)][strtolower($dep['name'])]
                  as $i => $p) {
                if ($p['channel'] == strtolower($pkg->getChannel()) &&
                      $p['package'] == strtolower($pkg->getPackage())) {
                    $found = true;
                    break;
                }
            }
            if (!$found) {
                $data['packages'][strtolower($depchannel)][strtolower($dep['name'])][]
                    = array('channel' => strtolower($pkg->getChannel()),
                            'package' => strtolower($pkg->getPackage()));
            }
        } else {
            $data['packages'][strtolower($depchannel)][strtolower($dep['name'])][]
                = array('channel' => strtolower($pkg->getChannel()),
                        'package' => strtolower($pkg->getPackage()));
        }
    }
}
?>