<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@php.net>                                   |
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// |          Martin Jansen <mj@php.net>                                  |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Common.php';
require_once 'PEAR/Registry.php';
require_once 'PEAR/Dependency.php';
require_once 'PEAR/Remote.php';
require_once 'System.php';


define('PEAR_INSTALLER_OK',       1);
define('PEAR_INSTALLER_FAILED',   0);
define('PEAR_INSTALLER_SKIPPED', -1);
define('PEAR_INSTALLER_ERROR_NO_PREF_STATE', 2);

/**
 * Administration class used to download PEAR packages and maintain the
 * installed package database.
 *
 * @since PEAR 1.4
 * @author Greg Beaver <cellog@php.net>
 */
class PEAR_Downloader extends PEAR_Common
{
    /**
     * @var PEAR_Config
     * @access private
     */
    var $_config;

    /**
     * @var PEAR_Registry
     * @access private
     */
    var $_registry;

    /**
     * @var PEAR_Remote
     * @access private
     */
    var $_remote;

    /**
     * Preferred Installation State (snapshot, devel, alpha, beta, stable)
     * @var string|null
     * @access private
     */
    var $_preferredState;

    /**
     * Options from command-line passed to Install.
     *
     * Recognized options:<br />
     *  - onlyreqdeps   : install all required dependencies as well
     *  - alldeps       : install all dependencies, including optional
     *  - installroot   : base relative path to install files in
     *  - force         : force a download even if warnings would prevent it
     * @see PEAR_Command_Install
     * @access private
     * @var array
     */
    var $_options;

    /**
     * Downloaded Packages after a call to download().
     *
     * Format of each entry:
     *
     * <code>
     * array('pkg' => 'package_name', 'file' => '/path/to/local/file',
     *    'info' => array() // parsed package.xml
     * );
     * </code>
     * @access private
     * @var array
     */
    var $_downloadedPackages = array();

    /**
     * Packages slated for download.
     *
     * This is used to prevent downloading a package more than once should it be a dependency
     * for two packages to be installed.
     * Format of each entry:
     *
     * <pre>
     * array('package_name1' => parsed package.xml, 'package_name2' => parsed package.xml,
     * );
     * </pre>
     * @access private
     * @var array
     */
    var $_toDownload = array();

    /**
     * Array of every package installed, with names lower-cased.
     *
     * Format:
     * <code>
     * array('package1' => 0, 'package2' => 1, );
     * </code>
     * @var array
     */
    var $_installed = array();

    /**
     * @var array
     * @access private
     */
    var $_errorStack = array();

    // {{{ PEAR_Downloader()

    function PEAR_Downloader(&$ui, $options, &$config)
    {
        $this->_options = $options;
        $this->_config = &$config;
        $this->_preferredState = $this->_config->get('preferred_state');
        $this->ui = &$ui;
        if (!$this->_preferredState) {
            // don't inadvertantly use a non-set preferred_state
            $this->_preferredState = null;
        }

        $php_dir = $this->_config->get('php_dir');
        if (isset($this->_options['installroot'])) {
            if (substr($this->_options['installroot'], -1) == DIRECTORY_SEPARATOR) {
                $this->_options['installroot'] = substr($this->_options['installroot'], 0, -1);
            }
            $php_dir = $this->_prependPath($php_dir, $this->_options['installroot']);
        }
        $this->_registry = &new PEAR_Registry($php_dir);
        $this->_remote = &new PEAR_Remote($config);

        if (isset($this->_options['alldeps']) || isset($this->_options['onlyreqdeps'])) {
            $this->_installed = $this->_registry->listPackages();
            array_walk($this->_installed, create_function('&$v,$k','$v = strtolower($v);'));
            $this->_installed = array_flip($this->_installed);
        }
        parent::PEAR_Common();
    }

    // }}}
    // {{{ configSet()
    function configSet($key, $value, $layer = 'user')
    {
        $this->_config->set($key, $value, $layer);
        $this->_preferredState = $this->_config->get('preferred_state');
        if (!$this->_preferredState) {
            // don't inadvertantly use a non-set preferred_state
            $this->_preferredState = null;
        }
    }

    // }}}
    // {{{ setOptions()
    function setOptions($options)
    {
        $this->_options = $options;
    }

    // }}}
    // {{{ _downloadFile()
    /**
     * @param string filename to download
     * @param string version/state
     * @param string original value passed to command-line
     * @param string|null preferred state (snapshot/devel/alpha/beta/stable)
     *                    Defaults to configuration preferred state
     * @return null|PEAR_Error|string
     * @access private
     */
    function _downloadFile($pkgfile, $version, $origpkgfile, $state = null)
    {
        if (is_null($state)) {
            $state = $this->_preferredState;
        }
        // {{{ check the package filename, and whether it's already installed
        $need_download = false;
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!@is_file($pkgfile)) {
            if ($this->validPackageName($pkgfile)) {
                if ($this->_registry->packageExists($pkgfile)) {
                    if (empty($this->_options['upgrade']) && empty($this->_options['force'])) {
                        $errors[] = "$pkgfile already installed";
                        return;
                    }
                }
                $pkgfile = $this->getPackageDownloadUrl($pkgfile, $version);
                $need_download = true;
            } else {
                if (strlen($pkgfile)) {
                    $errors[] = "Could not open the package file: $pkgfile";
                } else {
                    $errors[] = "No package file given";
                }
                return;
            }
        }
        // }}}

        // {{{ Download package -----------------------------------------------
        if ($need_download) {
            $downloaddir = $this->_config->get('download_dir');
            if (empty($downloaddir)) {
                if (PEAR::isError($downloaddir = System::mktemp('-d'))) {
                    return $downloaddir;
                }
                $this->log(3, '+ tmp dir created at ' . $downloaddir);
            }
            $callback = $this->ui ? array(&$this, '_downloadCallback') : null;
            $this->pushErrorHandling(PEAR_ERROR_RETURN);
            $file = $this->downloadHttp($pkgfile, $this->ui, $downloaddir, $callback);
            $this->popErrorHandling();
            if (PEAR::isError($file)) {
                if ($this->validPackageName($origpkgfile)) {
                    if (!PEAR::isError($info = $this->_remote->call('package.info',
                          $origpkgfile))) {
                        if (!count($info['releases'])) {
                            return $this->raiseError('Package ' . $origpkgfile .
                            ' has no releases');
                        } else {
                            return $this->raiseError('No releases of preferred state "'
                            . $state . '" exist for package ' . $origpkgfile .
                            '.  Use ' . $origpkgfile . '-state to install another' .
                            ' state (like ' . $origpkgfile .'-beta)',
                            PEAR_INSTALLER_ERROR_NO_PREF_STATE);
                        }
                    } else {
                        return $pkgfile;
                    }
                } else {
                    return $this->raiseError($file);
                }
            }
            $pkgfile = $file;
        }
        // }}}
        return $pkgfile;
    }
    // }}}
    // {{{ getPackageDownloadUrl()

    function getPackageDownloadUrl($package, $version = null)
    {
        if ($version) {
            $package .= "-$version";
        }
        if ($this === null || $this->_config === null) {
            $package = "http://pear.php.net/get/$package";
        } else {
            $package = "http://" . $this->_config->get('master_server') .
                "/get/$package";
        }
        if (!extension_loaded("zlib")) {
            $package .= '?uncompress=yes';
        }
        return $package;
    }

    // }}}
    // {{{ extractDownloadFileName($pkgfile, &$version)

    function extractDownloadFileName($pkgfile, &$version)
    {
        if (@is_file($pkgfile)) {
            return $pkgfile;
        }
        // regex defined in Common.php
        if (preg_match(PEAR_COMMON_PACKAGE_DOWNLOAD_PREG, $pkgfile, $m)) {
            $version = (isset($m[3])) ? $m[3] : null;
            return $m[1];
        }
        $version = null;
        return $pkgfile;
    }

    // }}}

    // }}}
    // {{{ getDownloadedPackages()

    /**
     * Retrieve a list of downloaded packages after a call to {@link download()}.
     *
     * Also resets the list of downloaded packages.
     * @return array
     */
    function getDownloadedPackages()
    {
        $ret = $this->_downloadedPackages;
        $this->_downloadedPackages = array();
        $this->_toDownload = array();
        return $ret;
    }

    // }}}
    // {{{ download()

    /**
     * Download any files and their dependencies, if necessary
     *
     * BC-compatible method name
     * @param array a mixed list of package names, local files, or package.xml
     */
    function download($packages)
    {
        return $this->doDownload($packages);
    }

    // }}}
    // {{{ doDownload()

    /**
     * Download any files and their dependencies, if necessary
     *
     * @param array a mixed list of package names, local files, or package.xml
     */
    function doDownload($packages)
    {
        $mywillinstall = array();
        $state = $this->_preferredState;

        // {{{ download files in this list if necessary
        foreach($packages as $pkgfile) {
            $need_download = false;
            if (!is_file($pkgfile)) {
                if (preg_match('#^(http|ftp)://#', $pkgfile)) {
                    $need_download = true;
                }
                $pkgfile = $this->_downloadNonFile($pkgfile);
                if (PEAR::isError($pkgfile)) {
                    return $pkgfile;
                }
                if ($pkgfile === false) {
                    continue;
                }
            } // end is_file()

            $tempinfo = $this->infoFromAny($pkgfile);
            if ($need_download) {
                $this->_toDownload[] = $tempinfo['package'];
            }
            if (isset($this->_options['alldeps']) || isset($this->_options['onlyreqdeps'])) {
                // ignore dependencies if there are any errors
                if (!PEAR::isError($tempinfo)) {
                    $mywillinstall[strtolower($tempinfo['package'])] = @$tempinfo['release_deps'];
                }
            }
            $this->_downloadedPackages[] = array('pkg' => $tempinfo['package'],
                                       'file' => $pkgfile, 'info' => $tempinfo);
        } // end foreach($packages)
        // }}}

        // {{{ extract dependencies from downloaded files and then download
        // them if necessary
        if (isset($this->_options['alldeps']) || isset($this->_options['onlyreqdeps'])) {
            $deppackages = array();
            foreach ($mywillinstall as $package => $alldeps) {
                if (!is_array($alldeps)) {
                    // there are no dependencies
                    continue;
                }
                $fail = false;
                foreach ($alldeps as $info) {
                    if ($info['type'] != 'pkg') {
                        continue;
                    }
                    $ret = $this->_processDependency($package, $info, $mywillinstall);
                    if ($ret === false) {
                        continue;
                    }
                    if ($ret === 0) {
                        $fail = true;
                        continue;
                    }
                    if (PEAR::isError($ret)) {
                        return $ret;
                    }
                    $deppackages[] = $ret;
                } // foreach($alldeps
                if ($fail) {
                    $deppackages = array();
                }
            }

            if (count($deppackages)) {
                $this->doDownload($deppackages);
            }
        } // }}} if --alldeps or --onlyreqdeps
    }

    // }}}
    // {{{ _downloadNonFile($pkgfile)

    /**
     * @return false|PEAR_Error|string false if loop should be broken out of,
     *                                 string if the file was downloaded,
     *                                 PEAR_Error on exception
     * @access private
     */
    function _downloadNonFile($pkgfile)
    {
        $origpkgfile = $pkgfile;
        $state = null;
        $pkgfile = $this->extractDownloadFileName($pkgfile, $version);
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            return $this->_downloadFile($pkgfile, $version, $origpkgfile);
        }
        if (!$this->validPackageName($pkgfile)) {
            return $this->raiseError("Package name '$pkgfile' not valid");
        }
        // ignore packages that are installed unless we are upgrading
        $curinfo = $this->_registry->packageInfo($pkgfile);
        if ($this->_registry->packageExists($pkgfile)
              && empty($this->_options['upgrade']) && empty($this->_options['force'])) {
            $this->log(0, "Package '{$curinfo['package']}' already installed, skipping");
            return false;
        }
        if (in_array($pkgfile, $this->_toDownload)) {
            return false;
        }
        $releases = $this->_remote->call('package.info', $pkgfile, 'releases', true);
        if (!count($releases)) {
            return $this->raiseError("No releases found for package '$pkgfile'");
        }
        // Want a specific version/state
        if ($version !== null) {
            // Passed Foo-1.2
            if ($this->validPackageVersion($version)) {
                if (!isset($releases[$version])) {
                    return $this->raiseError("No release with version '$version' found for '$pkgfile'");
                }
            // Passed Foo-alpha
            } elseif (in_array($version, $this->getReleaseStates())) {
                $state = $version;
                $version = 0;
                foreach ($releases as $ver => $inf) {
                    if ($inf['state'] == $state && version_compare("$version", "$ver") < 0) {
                        $version = $ver;
                        break;
                    }
                }
                if ($version == 0) {
                    return $this->raiseError("No release with state '$state' found for '$pkgfile'");
                }
            // invalid suffix passed
            } else {
                return $this->raiseError("Invalid suffix '-$version', be sure to pass a valid PEAR ".
                                         "version number or release state");
            }
        // Guess what to download
        } else {
            $states = $this->betterStates($this->_preferredState, true);
            $possible = false;
            $version = 0;
            $higher_version = 0;
            $prev_hi_ver = 0;
            foreach ($releases as $ver => $inf) {
                if (in_array($inf['state'], $states) && version_compare("$version", "$ver") < 0) {
                    $version = $ver;
                    break;
                } else {
                    $ver = (string)$ver;
                    if (version_compare($prev_hi_ver, $ver) < 0) {
                        $prev_hi_ver = $higher_version = $ver;
                    }
				}
            }
            if ($version === 0 && !isset($this->_options['force'])) {
                return $this->raiseError('No release with state equal to: \'' . implode(', ', $states) .
                                         "' found for '$pkgfile'");
            } elseif ($version === 0) {
                $this->log(0, "Warning: $pkgfile is state '" . $releases[$higher_version]['state'] . "' which is less stable " .
                              "than state '$this->_preferredState'");
            }
        }
        // Check if we haven't already the version
        if (empty($this->_options['force']) && !is_null($curinfo)) {
            if ($curinfo['version'] == $version) {
                $this->log(0, "Package '{$curinfo['package']}-{$curinfo['version']}' already installed, skipping");
                return false;
            } elseif (version_compare("$version", "{$curinfo['version']}") < 0) {
                $this->log(0, "Package '{$curinfo['package']}' version '{$curinfo['version']}' " .
                              " is installed and {$curinfo['version']} is > requested '$version', skipping");
                return false;
            }
        }
        $this->_toDownload[] = $pkgfile;
        return $this->_downloadFile($pkgfile, $version, $origpkgfile, $state);
    }

    // }}}
    // {{{ _processDependency($package, $info, $mywillinstall)

    /**
     * Process a dependency, download if necessary
     * @param array dependency information from PEAR_Remote call
     * @param array packages that will be installed in this iteration
     * @return false|string|PEAR_Error
     * @access private
     * @todo Add test for relation 'lt'/'le' -> make sure that the dependency requested is
     *       in fact lower than the required value.  This will be very important for BC dependencies
     */
    function _processDependency($package, $info, $mywillinstall)
    {
        $state = $this->_preferredState;
        if (!isset($this->_options['alldeps']) && isset($info['optional']) &&
              $info['optional'] == 'yes') {
            // skip optional deps
            $this->log(0, "skipping Package '$package' optional dependency '$info[name]'");
            return false;
        }
        // {{{ get releases
        $releases = $this->_remote->call('package.info', $info['name'], 'releases', true);
        if (PEAR::isError($releases)) {
            return $releases;
        }
        if (!count($releases)) {
            if (!isset($this->_installed[strtolower($info['name'])])) {
                $this->pushError("Package '$package' dependency '$info[name]' ".
                            "has no releases");
            }
            return false;
        }
        $found = false;
        $save = $releases;
        while(count($releases) && !$found) {
            if (!empty($state) && $state != 'any') {
                list($release_version, $release) = each($releases);
                if ($state != $release['state'] &&
                    !in_array($release['state'], $this->betterStates($state)))
                {
                    // drop this release - it ain't stable enough
                    array_shift($releases);
                } else {
                    $found = true;
                }
            } else {
                $found = true;
            }
        }
        if (!count($releases) && !$found) {
            $get = array();
            foreach($save as $release) {
                $get = array_merge($get,
                    $this->betterStates($release['state'], true));
            }
            $savestate = array_shift($get);
            $this->pushError( "Release for '$package' dependency '$info[name]' " .
                "has state '$savestate', requires '$state'");
            return 0;
        }
        if (in_array(strtolower($info['name']), $this->_toDownload) ||
              isset($mywillinstall[strtolower($info['name'])])) {
            // skip upgrade check for packages we will install
            return false;
        }
        if (!isset($this->_installed[strtolower($info['name'])])) {
            // check to see if we can install the specific version required
            if ($info['rel'] == 'eq') {
                return $info['name'] . '-' . $info['version'];
            }
            // skip upgrade check for packages we don't have installed
            return $info['name'];
        }
        // }}}

        // {{{ see if a dependency must be upgraded
        $inst_version = $this->_registry->packageInfo($info['name'], 'version');
        if (!isset($info['version'])) {
            // this is a rel='has' dependency, check against latest
            if (version_compare($release_version, $inst_version, 'le')) {
                return false;
            } else {
                return $info['name'];
            }
        }
        if (version_compare($info['version'], $inst_version, 'le')) {
            // installed version is up-to-date
            return false;
        }
        return $info['name'];
    }

    // }}}
    // {{{ _downloadCallback()

    function _downloadCallback($msg, $params = null)
    {
        switch ($msg) {
            case 'saveas':
                $this->log(1, "downloading $params ...");
                break;
            case 'done':
                $this->log(1, '...done: ' . number_format($params, 0, '', ',') . ' bytes');
                break;
            case 'bytesread':
                static $bytes;
                if (empty($bytes)) {
                    $bytes = 0;
                }
                if (!($bytes % 10240)) {
                    $this->log(1, '.', false);
                }
                $bytes += $params;
                break;
            case 'start':
                $this->log(1, "Starting to download {$params[0]} (".number_format($params[1], 0, '', ',')." bytes)");
                break;
        }
        if (method_exists($this->ui, '_downloadCallback'))
            $this->ui->_downloadCallback($msg, $params);
    }

    // }}}
    // {{{ _prependPath($path, $prepend)

    function _prependPath($path, $prepend)
    {
        if (strlen($prepend) > 0) {
            if (OS_WINDOWS && preg_match('/^[a-z]:/i', $path)) {
                $path = $prepend . substr($path, 2);
            } else {
                $path = $prepend . $path;
            }
        }
        return $path;
    }
    // }}}
    // {{{ pushError($errmsg, $code)

    /**
     * @param string
     * @param integer
     */
    function pushError($errmsg, $code = -1)
    {
        array_push($this->_errorStack, array($errmsg, $code));
    }

    // }}}
    // {{{ getErrorMsgs()

    function getErrorMsgs()
    {
        $msgs = array();
        $errs = $this->_errorStack;
        foreach ($errs as $err) {
            $msgs[] = $err[0];
        }
        $this->_errorStack = array();
        return $msgs;
    }

    // }}}
}
// }}}

?>
