<?php
/**
 * PEAR_Dependency2, advanced dependency validation
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
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */

/**
 * Required for the PEAR_VALIDATE_* constants
 */
require_once 'PEAR/Validate.php';

/**
 * Dependency check for PEAR packages
 *
 * This class handles both version 1.0 and 2.0 dependencies
 * WARNING: *any* changes to this class must be duplicated in the
 * test_PEAR_Dependency2 class found in tests/PEAR_Dependency2/setup.php.inc,
 * or unit tests will not actually validate the changes
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @PEAR-VER@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_Dependency2
{
    /**
     * One of the PEAR_VALIDATE_* states
     * @see PEAR_VALIDATE_NORMAL
     * @var integer
     */
    var $_state;
    /**
     * Command-line options to install/upgrade/uninstall commands
     * @param array
     */
    var $_options;
    /**
     * @var OS_Guess
     */
    var $_os;
    /**
     * @var PEAR_Registry
     */
    var $_registry;
    /**
     * @var PEAR_Config
     */
    var $_config;
    /**
     * @var PEAR_DependencyDB
     */
    var $_dependencydb;
    /**
     * Output of PEAR_Registry::parsedPackageName()
     * @var array
     */
    var $_currentPackage;
    /**
     * @param PEAR_Config
     * @param array installation options
     * @param array format of PEAR_Registry::parsedPackageName()
     * @param int installation state (one of PEAR_VALIDATE_*)
     */
    function PEAR_Dependency2(&$config, $installoptions, $package,
                              $state = PEAR_VALIDATE_INSTALLING)
    {
        $this->_config = &$config;
        if (!class_exists('PEAR_DependencyDB')) {
            require_once 'PEAR/DependencyDB.php';
        }
        if (isset($installoptions['packagingroot'])) {
            // make sure depdb is in the right location
            $config->setInstallRoot($installoptions['packagingroot']);
        }
        $this->_registry = &$config->getRegistry();
        $this->_dependencydb = &PEAR_DependencyDB::singleton($config);
        if (isset($installoptions['packagingroot'])) {
            $config->setInstallRoot(false);
        }
        $this->_options = $installoptions;
        $this->_state = $state;
        if (!class_exists('OS_Guess')) {
            require_once 'OS/Guess.php';
        }
        $this->_os = new OS_Guess;
        $this->_currentPackage = $package;
    }

    function _getExtraString($dep)
    {
        $extra = ' (';
        if (isset($dep['uri'])) {
            return '';
        }
        if (isset($dep['recommended'])) {
            $extra .= 'recommended version ' . $dep['recommended'];
        } else {
            if (isset($dep['min'])) {
                $extra .= 'version >= ' . $dep['min'];
            }
            if (isset($dep['max'])) {
                if ($extra != ' (') {
                    $extra .= ', ';
                }
                $extra .= 'version <= ' . $dep['max'];
            }
            if (isset($dep['exclude'])) {
                if (!is_array($dep['exclude'])) {
                    $dep['exclude'] = array($dep['exclude']);
                }
                if ($extra != ' (') {
                    $extra .= ', ';
                }
                $extra .= 'excluded versions: ';
                foreach ($dep['exclude'] as $i => $exclude) {
                    if ($i) {
                        $extra .= ', ';
                    }
                    $extra .= $exclude;
                }
            }
        }
        $extra .= ')';
        if ($extra == ' ()') {
            $extra = '';
        }
        return $extra;
    }

    /**
     * This makes unit-testing a heck of a lot easier
     */
    function getPHP_OS()
    {
        return PHP_OS;
    }

    /**
     * This makes unit-testing a heck of a lot easier
     */
    function getsysname()
    {
        return $this->_os->getSysname();
    }

    /**
     * Specify a dependency on an OS.  Use arch for detailed os/processor information
     *
     * There are two generic OS dependencies that will be the most common, unix and windows.
     * Other options are linux, freebsd, darwin (OS X), sunos, irix, hpux, aix
     */
    function validateOsDependency($dep)
    {
        if ($this->_state != PEAR_VALIDATE_INSTALLING &&
              $this->_state != PEAR_VALIDATE_DOWNLOADING) {
            return true;
        }
        if (isset($dep['conflicts'])) {
            $not = true;
        } else {
            $not = false;
        }
        if ($dep['name'] == '*') {
            return true;
        }
        switch (strtolower($dep['name'])) {
            case 'windows' :
                if ($not) {
                    if (strtolower(substr($this->getPHP_OS(), 0, 3)) == 'win') {
                        if (!isset($this->_options['nodeps']) &&
                              !isset($this->_options['force'])) {
                            return $this->raiseError("Cannot install %s on Windows");
                        } else {
                            return $this->warning("warning: Cannot install %s on Windows");
                        }
                    }
                } else {
                    if (strtolower(substr($this->getPHP_OS(), 0, 3)) != 'win') {
                        if (!isset($this->_options['nodeps']) &&
                              !isset($this->_options['force'])) {
                            return $this->raiseError("Can only install %s on Windows");
                        } else {
                            return $this->warning("warning: Can only install %s on Windows");
                        }
                    }
                }
            break;
            case 'unix' :
                $unices = array('linux', 'freebsd', 'darwin', 'sunos', 'irix', 'hpux', 'aix');
                if ($not) {
                    if (in_array($this->getSysname(), $unices)) {
                        if (!isset($this->_options['nodeps']) &&
                              !isset($this->_options['force'])) {
                            return $this->raiseError("Cannot install %s on any Unix system");
                        } else {
                            return $this->warning(
                                "warning: Cannot install %s on any Unix system");
                        }
                    }
                } else {
                    if (!in_array($this->getSysname(), $unices)) {
                        if (!isset($this->_options['nodeps']) &&
                              !isset($this->_options['force'])) {
                            return $this->raiseError("Can only install %s on a Unix system");
                        } else {
                            return $this->warning(
                                "warning: Can only install %s on a Unix system");
                        }
                    }
                }
            break;
            default :
                if ($not) {
                    if (strtolower($dep['name']) == strtolower($this->getSysname())) {
                        if (!isset($this->_options['nodeps']) &&
                              !isset($this->_options['force'])) {
                            return $this->raiseError('Cannot install %s on ' . $dep['name'] .
                                ' operating system');
                        } else {
                            return $this->warning('warning: Cannot install %s on ' .
                                $dep['name'] . ' operating system');
                        }
                    }
                } else {
                    if (strtolower($dep['name']) != strtolower($this->getSysname())) {
                        if (!isset($this->_options['nodeps']) &&
                              !isset($this->_options['force'])) {
                            return $this->raiseError('Cannot install %s on ' .
                                $this->getSysname() .
                                ' operating system, can only install on ' . $dep['name']);
                        } else {
                            return $this->warning('warning: Cannot install %s on ' .
                                $this->getSysname() .
                                ' operating system, can only install on ' . $dep['name']);
                        }
                    }
                }
        }
        return true;
    }

    /**
     * This makes unit-testing a heck of a lot easier
     */
    function matchSignature($pattern)
    {
        return $this->_os->matchSignature($pattern);
    }

    /**
     * Specify a complex dependency on an OS/processor/kernel version,
     * Use OS for simple operating system dependency.
     *
     * This is the only dependency that accepts an eregable pattern.  The pattern
     * will be matched against the php_uname() output parsed by OS_Guess
     */
    function validateArchDependency($dep)
    {
        if ($this->_state != PEAR_VALIDATE_INSTALLING) {
            return true;
        }
        if (isset($dep['conflicts'])) {
            $not = true;
        } else {
            $not = false;
        }
        if (!$this->matchSignature($dep['pattern'])) {
            if (!$not) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s Architecture dependency failed, does not ' .
                        'match "' . $dep['pattern'] . '"');
                } else {
                    return $this->warning('warning: %s Architecture dependency failed, does ' .
                        'not match "' . $dep['pattern'] . '"');
                }
            }
            return true;
        } else {
            if ($not) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s Architecture dependency failed, required "' .
                        $dep['pattern'] . '"');
                } else {
                    return $this->warning('warning: %s Architecture dependency failed, ' .
                        'required "' . $dep['pattern'] . '"');
                }
            }
            return true;
        }
    }

    /**
     * This makes unit-testing a heck of a lot easier
     */
    function extension_loaded($name)
    {
        return extension_loaded($name);
    }

    /**
     * This makes unit-testing a heck of a lot easier
     */
    function phpversion($name = null)
    {
        if ($name !== null) {
            return phpversion($name);
        } else {
            return phpversion();
        }
    }

    function validateExtensionDependency($dep, $required = true)
    {
        if ($this->_state != PEAR_VALIDATE_INSTALLING &&
              $this->_state != PEAR_VALIDATE_DOWNLOADING) {
            return true;
        }
        $loaded = $this->extension_loaded($dep['name']);
        $extra = $this->_getExtraString($dep);
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
        }
        if (!isset($dep['min']) && !isset($dep['max']) &&
              !isset($dep['recommended']) && !isset($dep['exclude'])) {
            if ($loaded) {
                if (isset($dep['conflicts'])) {
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s conflicts with PHP extension "' .
                            $dep['name'] . '"' . $extra);
                    } else {
                        return $this->warning('warning: %s conflicts with PHP extension "' .
                            $dep['name'] . '"' . $extra);
                    }
                }
                return true;
            } else {
                if (isset($dep['conflicts'])) {
                    return true;
                }
                if ($required) {
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s requires PHP extension "' .
                            $dep['name'] . '"' . $extra);
                    } else {
                        return $this->warning('warning: %s requires PHP extension "' .
                            $dep['name'] . '"' . $extra);
                    }
                } else {
                    return $this->warning('%s can optionally use PHP extension "' .
                        $dep['name'] . '"' . $extra);
                }
            }
        }
        if (!$loaded) {
            if (isset($dep['conflicts'])) {
                return true;
            }
            if (!$required) {
                return $this->warning('%s can optionally use PHP extension "' .
                    $dep['name'] . '"' . $extra);
            } else {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s requires PHP extension "' . $dep['name'] .
                        '"' . $extra);
                }
                    return $this->warning('warning: %s requires PHP extension "' . $dep['name'] .
                        '"' . $extra);
            }
        }
        $version = (string) $this->phpversion($dep['name']);
        if (empty($version)) {
            $version = '0';
        }
        $fail = false;
        if (isset($dep['min'])) {
            if (!version_compare($version, $dep['min'], '>=')) {
                $fail = true;
            }
        }
        if (isset($dep['max'])) {
            if (!version_compare($version, $dep['max'], '<=')) {
                $fail = true;
            }
        }
        if ($fail && !isset($dep['conflicts'])) {
            if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                return $this->raiseError('%s requires PHP extension "' . $dep['name'] .
                    '"' . $extra . ', installed version is ' . $version);
            } else {
                return $this->warning('warning: %s requires PHP extension "' . $dep['name'] .
                    '"' . $extra . ', installed version is ' . $version);
            }
        } elseif ((isset($dep['min']) || isset($dep['max'])) && !$fail && isset($dep['conflicts'])) {
            if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                return $this->raiseError('%s conflicts with PHP extension "' .
                    $dep['name'] . '"' . $extra . ', installed version is ' . $version);
            } else {
                return $this->warning('warning: %s conflicts with PHP extension "' .
                    $dep['name'] . '"' . $extra . ', installed version is ' . $version);
            }
        }
        if (isset($dep['exclude'])) {
            foreach ($dep['exclude'] as $exclude) {
                if (version_compare($version, $exclude, '==')) {
                    if (isset($dep['conflicts'])) {
                        continue;
                    }
                    if (!isset($this->_options['nodeps']) &&
                          !isset($this->_options['force'])) {
                        return $this->raiseError('%s is not compatible with PHP extension "' .
                            $dep['name'] . '" version ' .
                            $exclude);
                    } else {
                        return $this->warning('warning: %s is not compatible with PHP extension "' .
                            $dep['name'] . '" version ' .
                            $exclude);
                    }
                } elseif (version_compare($version, $exclude, '!=') && isset($dep['conflicts'])) {
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s conflicts with PHP extension "' .
                            $dep['name'] . '"' . $extra . ', installed version is ' . $version);
                    } else {
                        return $this->warning('warning: %s conflicts with PHP extension "' .
                            $dep['name'] . '"' . $extra . ', installed version is ' . $version);
                    }
                }
            }
        }
        if (isset($dep['recommended'])) {
            if (version_compare($version, $dep['recommended'], '==')) {
                return true;
            } else {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s dependency: PHP extension ' . $dep['name'] .
                        ' version "' . $version . '"' .
                        ' is not the recommended version "' . $dep['recommended'] .
                        '", but may be compatible, use --force to install');
                } else {
                    return $this->warning('warning: %s dependency: PHP extension ' .
                        $dep['name'] . ' version "' . $version . '"' .
                        ' is not the recommended version "' . $dep['recommended'].'"');
                }
            }
        }
        return true;
    }

    function validatePhpDependency($dep)
    {
        if ($this->_state != PEAR_VALIDATE_INSTALLING &&
              $this->_state != PEAR_VALIDATE_DOWNLOADING) {
            return true;
        }
        $version = $this->phpversion();
        $extra = $this->_getExtraString($dep);
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
        }
        if (isset($dep['min'])) {
            if (!version_compare($version, $dep['min'], '>=')) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s requires PHP' .
                        $extra . ', installed version is ' . $version);
                } else {
                    return $this->warning('warning: %s requires PHP' .
                        $extra . ', installed version is ' . $version);
                }
            }
        }
        if (isset($dep['max'])) {
            if (!version_compare($version, $dep['max'], '<=')) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s requires PHP' .
                        $extra . ', installed version is ' . $version);
                } else {
                    return $this->warning('warning: %s requires PHP' .
                        $extra . ', installed version is ' . $version);
                }
            }
        }
        if (isset($dep['exclude'])) {
            foreach ($dep['exclude'] as $exclude) {
                if (version_compare($version, $exclude, '==')) {
                    if (!isset($this->_options['nodeps']) &&
                          !isset($this->_options['force'])) {
                        return $this->raiseError('%s is not compatible with PHP version ' .
                            $exclude);
                    } else {
                        return $this->warning(
                            'warning: %s is not compatible with PHP version ' .
                            $exclude);
                    }
                }
            }
        }
        return true;
    }

    /**
     * This makes unit-testing a heck of a lot easier
     */
    function getPEARVersion()
    {
        return '1.4.9';
    }

    function validatePearinstallerDependency($dep)
    {
        $pearversion = $this->getPEARVersion();
        $extra = $this->_getExtraString($dep);
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
        }
        if (version_compare($pearversion, $dep['min'], '<')) {
            if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                return $this->raiseError('%s requires PEAR Installer' . $extra .
                    ', installed version is ' . $pearversion);
            } else {
                return $this->warning('warning: %s requires PEAR Installer' . $extra .
                    ', installed version is ' . $pearversion);
            }
        }
        if (isset($dep['max'])) {
            if (version_compare($pearversion, $dep['max'], '>')) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s requires PEAR Installer' . $extra .
                        ', installed version is ' . $pearversion);
                } else {
                    return $this->warning('warning: %s requires PEAR Installer' . $extra .
                        ', installed version is ' . $pearversion);
                }
            }
        }
        if (isset($dep['exclude'])) {
            if (!isset($dep['exclude'][0])) {
                $dep['exclude'] = array($dep['exclude']);
            }
            foreach ($dep['exclude'] as $exclude) {
                if (version_compare($exclude, $pearversion, '==')) {
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s is not compatible with PEAR Installer ' .
                            'version ' . $exclude);
                    } else {
                        return $this->warning('warning: %s is not compatible with PEAR ' .
                            'Installer version ' . $exclude);
                    }
                }
            }
        }
        return true;
    }

    function validateSubpackageDependency($dep, $required, $params)
    {
        return $this->validatePackageDependency($dep, $required, $params);
    }

    /**
     * @param array dependency information (2.0 format)
     * @param boolean whether this is a required dependency
     * @param array a list of downloaded packages to be installed, if any
     * @param boolean if true, then deps on pear.php.net that fail will also check
     *                against pecl.php.net packages to accomodate extensions that have
     *                moved to pecl.php.net from pear.php.net
     */
    function validatePackageDependency($dep, $required, $params, $depv1 = false)
    {
        if ($this->_state != PEAR_VALIDATE_INSTALLING &&
              $this->_state != PEAR_VALIDATE_DOWNLOADING) {
            return true;
        }
        if (isset($dep['providesextension'])) {
            if ($this->extension_loaded($dep['providesextension'])) {
                $save = $dep;
                $subdep = $dep;
                $subdep['name'] = $subdep['providesextension'];
                PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
                $ret = $this->validateExtensionDependency($subdep, $required);
                PEAR::popErrorHandling();
                if (!PEAR::isError($ret)) {
                    return true;
                }
            }
        }
        if ($this->_state == PEAR_VALIDATE_INSTALLING) {
            return $this->_validatePackageInstall($dep, $required, $depv1);
        }
        if ($this->_state == PEAR_VALIDATE_DOWNLOADING) {
            return $this->_validatePackageDownload($dep, $required, $params, $depv1);
        }
    }

    function _validatePackageDownload($dep, $required, $params, $depv1 = false)
    {
        $dep['package'] = $dep['name'];
        if (isset($dep['uri'])) {
            $dep['channel'] = '__uri';
        }
        $depname = $this->_registry->parsedPackageNameToString($dep, true);
        $found = false;
        foreach ($params as $param) {
            if ($param->isEqual(
                  array('package' => $dep['name'],
                        'channel' => $dep['channel']))) {
                $found = true;
                break;
            }
            if ($depv1 && $dep['channel'] == 'pear.php.net') {
                if ($param->isEqual(
                  array('package' => $dep['name'],
                        'channel' => 'pecl.php.net'))) {
                    $found = true;
                    break;
                }
            }
        }
        if (!$found && isset($dep['providesextension'])) {
            foreach ($params as $param) {
                if ($param->isExtension($dep['providesextension'])) {
                    $found = true;
                    break;
                }
            }
        }
        if ($found) {
            $version = $param->getVersion();
            $installed = false;
            $downloaded = true;
        } else {
            if ($this->_registry->packageExists($dep['name'], $dep['channel'])) {
                $installed = true;
                $downloaded = false;
                $version = $this->_registry->packageinfo($dep['name'], 'version',
                    $dep['channel']);
            } else {
                if ($dep['channel'] == 'pecl.php.net' && $this->_registry->packageExists($dep['name'],
                      'pear.php.net')) {
                    $installed = true;
                    $downloaded = false;
                    $version = $this->_registry->packageinfo($dep['name'], 'version',
                        'pear.php.net');
                } else {
                    $version = 'not installed or downloaded';
                    $installed = false;
                    $downloaded = false;
                }
            }
        }
        $extra = $this->_getExtraString($dep);
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
        }
        if (!isset($dep['min']) && !isset($dep['max']) &&
              !isset($dep['recommended']) && !isset($dep['exclude'])) {
            if ($installed || $downloaded) {
                $installed = $installed ? 'installed' : 'downloaded';
                if (isset($dep['conflicts'])) {
                    if ($version) {
                        $rest = ", $installed version is " . $version;
                    } else {
                        $rest = '';
                    }
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s conflicts with package "' . $depname . '"' .
                            $extra . $rest);
                    } else {
                        return $this->warning('warning: %s conflicts with package "' . $depname . '"' .
                            $extra . $rest);
                    }
                }
                return true;
            } else {
                if (isset($dep['conflicts'])) {
                    return true;
                }
                if ($required) {
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s requires package "' . $depname . '"' .
                            $extra);
                    } else {
                        return $this->warning('warning: %s requires package "' . $depname . '"' .
                            $extra);
                    }
                } else {
                    return $this->warning('%s can optionally use package "' . $depname . '"' .
                        $extra);
                }
            }
        }
        if (!$installed && !$downloaded) {
            if (isset($dep['conflicts'])) {
                return true;
            }
            if ($required) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s requires package "' . $depname . '"' .
                        $extra);
                } else {
                    return $this->warning('warning: %s requires package "' . $depname . '"' .
                        $extra);
                }
            } else {
                return $this->warning('%s can optionally use package "' . $depname . '"' .
                    $extra);
            }
        }
        $fail = false;
        if (isset($dep['min'])) {
            if (version_compare($version, $dep['min'], '<')) {
                $fail = true;
            }
        }
        if (isset($dep['max'])) {
            if (version_compare($version, $dep['max'], '>')) {
                $fail = true;
            }
        }
        if ($fail && !isset($dep['conflicts'])) {
            $installed = $installed ? 'installed' : 'downloaded';
            $dep['package'] = $dep['name'];
            $dep = $this->_registry->parsedPackageNameToString($dep, true);
            if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                return $this->raiseError('%s requires package "' . $depname . '"' .
                    $extra . ", $installed version is " . $version);
            } else {
                return $this->warning('warning: %s requires package "' . $depname . '"' .
                    $extra . ", $installed version is " . $version);
            }
        } elseif ((isset($dep['min']) || isset($dep['max'])) && !$fail &&
              isset($dep['conflicts']) && !isset($dep['exclude'])) {
            $installed = $installed ? 'installed' : 'downloaded';
            if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                return $this->raiseError('%s conflicts with package "' . $depname . '"' . $extra .
                    ", $installed version is " . $version);
            } else {
                return $this->warning('warning: %s conflicts with package "' . $depname . '"' .
                    $extra . ", $installed version is " . $version);
            }
        }
        if (isset($dep['exclude'])) {
            $installed = $installed ? 'installed' : 'downloaded';
            foreach ($dep['exclude'] as $exclude) {
                if (version_compare($version, $exclude, '==') && !isset($dep['conflicts'])) {
                    if (!isset($this->_options['nodeps']) &&
                          !isset($this->_options['force'])) {
                        return $this->raiseError('%s is not compatible with ' .
                            $installed . ' package "' .
                            $depname . '" version ' .
                            $exclude);
                    } else {
                        return $this->warning('warning: %s is not compatible with ' .
                            $installed . ' package "' .
                            $depname . '" version ' .
                            $exclude);
                    }
                } elseif (version_compare($version, $exclude, '!=') && isset($dep['conflicts'])) {
                    $installed = $installed ? 'installed' : 'downloaded';
                    if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                        return $this->raiseError('%s conflicts with package "' . $depname . '"' .
                            $extra . ", $installed version is " . $version);
                    } else {
                        return $this->warning('warning: %s conflicts with package "' . $depname . '"' .
                            $extra . ", $installed version is " . $version);
                    }
                }
            }
        }
        if (isset($dep['recommended'])) {
            $installed = $installed ? 'installed' : 'downloaded';
            if (version_compare($version, $dep['recommended'], '==')) {
                return true;
            } else {
                if (!$found && $installed) {
                    $param = $this->_registry->getPackage($dep['name'], $dep['channel']);
                }
                if ($param) {
                    $found = false;
                    foreach ($params as $parent) {
                        if ($parent->isEqual($this->_currentPackage)) {
                            $found = true;
                            break;
                        }
                    }
                    if ($found) {
                        if ($param->isCompatible($parent)) {
                            return true;
                        }
                    } else { // this is for validPackage() calls
                        $parent = $this->_registry->getPackage($this->_currentPackage['package'],
                            $this->_currentPackage['channel']);
                        if ($parent !== null) {
                            if ($param->isCompatible($parent)) {
                                return true;
                            }
                        }
                    }
                }
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force']) &&
                      !isset($this->_options['loose'])) {
                    return $this->raiseError('%s dependency package "' . $depname .
                        '" ' . $installed . ' version ' . $version . 
                        ' is not the recommended version ' . $dep['recommended'] .
                        ', but may be compatible, use --force to install');
                } else {
                    return $this->warning('warning: %s dependency package "' . $depname .
                        '" ' . $installed . ' version ' . $version .
                        ' is not the recommended version ' . $dep['recommended']);
                }
            }
        }
        return true;
    }

    function _validatePackageInstall($dep, $required, $depv1 = false)
    {
        return $this->_validatePackageDownload($dep, $required, array(), $depv1);
    }

    /**
     * Verify that uninstalling packages passed in to command line is OK.
     *
     * @param PEAR_Installer $dl
     * @return PEAR_Error|true
     */
    function validatePackageUninstall(&$dl)
    {
        if (PEAR::isError($this->_dependencydb)) {
            return $this->_dependencydb;
        }
        $params = array();
        // construct an array of "downloaded" packages to fool the package dependency checker
        // into using these to validate uninstalls of circular dependencies
        $downloaded = &$dl->getUninstallPackages();
        foreach ($downloaded as $i => $pf) {
            if (!class_exists('PEAR_Downloader_Package')) {
                require_once 'PEAR/Downloader/Package.php';
            }
            $dp = &new PEAR_Downloader_Package($dl);
            $dp->setPackageFile($downloaded[$i]);
            $params[$i] = &$dp;
        }
        $deps = $this->_dependencydb->getDependentPackageDependencies($this->_currentPackage);
        $fail = false;
        if ($deps) {
            foreach ($deps as $channel => $info) {
                foreach ($info as $package => $ds) {
                    foreach ($ds as $d) {
                        $d['dep']['package'] = $d['dep']['name'];
                        $checker = &new PEAR_Dependency2($this->_config, $this->_options,
                            array('channel' => $channel, 'package' => $package), $this->_state);
                        $dep = $d['dep'];
                        $required = $d['type'] == 'required';
                        $ret = $checker->_validatePackageUninstall($dep, $required, $params, $dl);
                        if (is_array($ret)) {
                            $dl->log(0, $ret[0]);
                        } elseif (PEAR::isError($ret)) {
                            $dl->log(0, $ret->getMessage());
                            $fail = true;
                        }
                    }
                }
            }
        }
        if ($fail) {
            if (isset($this->_options['nodeps']) || isset($this->_options['force'])) {
                return $this->warning(
                    'warning: %s should not be uninstalled, other installed packages depend ' .
                    'on this package');
            } else {
                return $this->raiseError(
                    '%s cannot be uninstalled, other installed packages depend on this package');
            }
        }
        return true;
    }

    function _validatePackageUninstall($dep, $required, $params, &$dl)
    {
        $dep['package'] = $dep['name'];
        $depname = $this->_registry->parsedPackageNameToString($dep, true);
        $found = false;
        foreach ($params as $param) {
            if ($param->isEqual($this->_currentPackage)) {
                $found = true;
                break;
            }
        }
        $version = $this->_registry->packageinfo($dep['name'], 'version',
            $dep['channel']);
        if (!$version) {
            return true;
        }
        $extra = $this->_getExtraString($dep);
        if (isset($dep['exclude'])) {
            if (!is_array($dep['exclude'])) {
                $dep['exclude'] = array($dep['exclude']);
            }
        }
        if (isset($dep['conflicts'])) {
            return true; // uninstall OK - these packages conflict (probably installed with --force)
        }
        if (!isset($dep['min']) && !isset($dep['max'])) {
            if ($required) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError('%s' . $extra . ' is required by installed package "' .
                        $depname . '"');
                } else {
                    return $this->warning('warning: %s' . $extra .
                        ' is required by installed package "' . $depname . '"');
                }
            } else {
                return $this->warning('%s' . $extra .
                    ' can be optionally used by installed package "' . $depname . '"');
            }
        }
        $fail = false;
        if (isset($dep['min'])) {
            if (version_compare($version, $dep['min'], '>=')) {
                $fail = true;
            }
        }
        if (isset($dep['max'])) {
            if (version_compare($version, $dep['max'], '<=')) {
                $fail = true;
            }
        }
        if ($fail) {
            if ($found) {
                if (!isset($dl->___checked[$this->_currentPackage['channel']]
                      [$this->_currentPackage['package']])) {
                    $dl->___checked[$this->_currentPackage['channel']]
                      [$this->_currentPackage['package']] = true;
                    $deps = $this->_dependencydb->getDependentPackageDependencies(
                        $this->_currentPackage);
                    if ($deps) {
                        foreach ($deps as $channel => $info) {
                            foreach ($info as $package => $ds) {
                                foreach ($ds as $d) {
                                    $d['dep']['package'] = $d['dep']['name'];
                                    $checker = &new PEAR_Dependency2($this->_config, $this->_options,
                                        array('channel' => $channel, 'package' => $package),
                                        $this->_state);
                                    $dep = $d['dep'];
                                    $required = $d['type'] == 'required';
                                    $ret = $checker->_validatePackageUninstall($dep, $required, $params,
                                        $dl);
                                    if (PEAR::isError($ret)) {
                                        $fail = true;
                                        break 3;
                                    }
                                }
                            }
                            $fail = false;
                        }
                    }
                } else {
                    return true;
                }
            }
            if (!$fail) {
                return true;
            }
            if ($required) {
                if (!isset($this->_options['nodeps']) && !isset($this->_options['force'])) {
                    return $this->raiseError($depname . $extra . ' is required by installed package' .
                        ' "%s"');
                } else {
                    return $this->warning('warning: ' . $depname . $extra .
                        ' is required by installed package "%s"');
                }
            } else {
                return $this->warning($depname . $extra . ' can be optionally used by installed package' .
                        ' "%s"');
            }
        }
        return true;
    }

    /**
     * validate a downloaded package against installed packages
     * 
     * As of PEAR 1.4.3, this will only validate
     *
     * @param array|PEAR_Downloader_Package|PEAR_PackageFile_v1|PEAR_PackageFile_v2
     *              $pkg package identifier (either
     *                   array('package' => blah, 'channel' => blah) or an array with
     *                   index 'info' referencing an object)
     * @param PEAR_Downloader $dl
     * @param array $params full list of packages to install
     * @return true|PEAR_Error
     */
    function validatePackage($pkg, &$dl, $params = array())
    {
        if (is_array($pkg) && isset($pkg['info'])) {
            $deps = $this->_dependencydb->getDependentPackageDependencies($pkg['info']);
        } else {
            $deps = $this->_dependencydb->getDependentPackageDependencies($pkg);
        }
        $fail = false;
        if ($deps) {
            if (!class_exists('PEAR_Downloader_Package')) {
                require_once 'PEAR/Downloader/Package.php';
            }
            $dp = &new PEAR_Downloader_Package($dl);
            if (is_object($pkg)) {
                $dp->setPackageFile($pkg);
            } else {
                $dp->setDownloadURL($pkg);
            }
            PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
            foreach ($deps as $channel => $info) {
                foreach ($info as $package => $ds) {
                    foreach ($params as $packd) {
                        if (strtolower($packd->getPackage()) == strtolower($package) &&
                              $packd->getChannel() == $channel) {
                            $dl->log(3, 'skipping installed package check of "' .
                                        $this->_registry->parsedPackageNameToString(
                                            array('channel' => $channel, 'package' => $package),
                                            true) .
                                        '", version "' . $packd->getVersion() . '" will be ' .
                                        'downloaded and installed');
                            continue 2; // jump to next package
                        }
                    }
                    foreach ($ds as $d) {
                        $checker = &new PEAR_Dependency2($this->_config, $this->_options,
                            array('channel' => $channel, 'package' => $package), $this->_state);
                        $dep = $d['dep'];
                        $required = $d['type'] == 'required';
                        $ret = $checker->_validatePackageDownload($dep, $required, array(&$dp));
                        if (is_array($ret)) {
                            $dl->log(0, $ret[0]);
                        } elseif (PEAR::isError($ret)) {
                            $dl->log(0, $ret->getMessage());
                            $fail = true;
                        }
                    }
                }
            }
            PEAR::popErrorHandling();
        }
        if ($fail) {
            return $this->raiseError(
                '%s cannot be installed, conflicts with installed packages');
        }
        return true;
    }

    /**
     * validate a package.xml 1.0 dependency
     */
    function validateDependency1($dep, $params = array())
    {
        if (!isset($dep['optional'])) {
            $dep['optional'] = 'no';
        }
        list($newdep, $type) = $this->normalizeDep($dep);
        if (!$newdep) {
            return $this->raiseError("Invalid Dependency");
        }
        if (method_exists($this, "validate{$type}Dependency")) {
            return $this->{"validate{$type}Dependency"}($newdep, $dep['optional'] == 'no',
                $params, true);
        }
    }

    /**
     * Convert a 1.0 dep into a 2.0 dep
     */
    function normalizeDep($dep)
    {
        $types = array(
            'pkg' => 'Package',
            'ext' => 'Extension',
            'os' => 'Os',
            'php' => 'Php'
        );
        if (isset($types[$dep['type']])) {
            $type = $types[$dep['type']];
        } else {
            return array(false, false);
        }
        $newdep = array();
        switch ($type) {
            case 'Package' :
                $newdep['channel'] = 'pear.php.net';
            case 'Extension' :
            case 'Os' :
                $newdep['name'] = $dep['name'];
            break;
        }
        $dep['rel'] = PEAR_Dependency2::signOperator($dep['rel']);
        switch ($dep['rel']) {
            case 'has' :
                return array($newdep, $type);
            break;
            case 'not' :
                $newdep['conflicts'] = true;
            break;
            case '>=' :
            case '>' :
                $newdep['min'] = $dep['version'];
                if ($dep['rel'] == '>') {
                    $newdep['exclude'] = $dep['version'];
                }
            break;
            case '<=' :
            case '<' :
                $newdep['max'] = $dep['version'];
                if ($dep['rel'] == '<') {
                    $newdep['exclude'] = $dep['version'];
                }
            break;
            case 'ne' :
            case '!=' :
                $newdep['min'] = '0';
                $newdep['max'] = '100000';
                $newdep['exclude'] = $dep['version'];
            break;
            case '==' :
                $newdep['min'] = $dep['version'];
                $newdep['max'] = $dep['version'];
            break;
        }
        if ($type == 'Php') {
            if (!isset($newdep['min'])) {
                $newdep['min'] = '4.2.0';
            }
            if (!isset($newdep['max'])) {
                $newdep['max'] = '6.0.0';
            }
        }
        return array($newdep, $type);
    }

    /**
     * Converts text comparing operators to them sign equivalents
     *
     * Example: 'ge' to '>='
     *
     * @access public
     * @param  string Operator
     * @return string Sign equivalent
     */
    function signOperator($operator)
    {
        switch($operator) {
            case 'lt': return '<';
            case 'le': return '<=';
            case 'gt': return '>';
            case 'ge': return '>=';
            case 'eq': return '==';
            case 'ne': return '!=';
            default:
                return $operator;
        }
    }

    function raiseError($msg)
    {
        if (isset($this->_options['ignore-errors'])) {
            return $this->warning($msg);
        }
        return PEAR::raiseError(sprintf($msg, $this->_registry->parsedPackageNameToString(
            $this->_currentPackage, true)));
    }

    function warning($msg)
    {
        return array(sprintf($msg, $this->_registry->parsedPackageNameToString(
            $this->_currentPackage, true)));
    }
}
?>