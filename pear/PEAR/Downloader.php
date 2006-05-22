<?php
/**
 * PEAR_Downloader, the PEAR Installer's download utility class
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
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V. V. Cox <cox@idecnet.com>
 * @author     Martin Jansen <mj@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.3.0
 */

/**
 * Needed for constants, extending
 */
require_once 'PEAR/Common.php';

define('PEAR_INSTALLER_OK',       1);
define('PEAR_INSTALLER_FAILED',   0);
define('PEAR_INSTALLER_SKIPPED', -1);
define('PEAR_INSTALLER_ERROR_NO_PREF_STATE', 2);

/**
 * Administration class used to download anything from the internet (PEAR Packages,
 * static URLs, xml files)
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V. V. Cox <cox@idecnet.com>
 * @author     Martin Jansen <mj@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.3.0
 */
class PEAR_Downloader extends PEAR_Common
{
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
     *  - nocompress    : download uncompressed tarballs
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
    
    /**
     * @var boolean
     * @access private
     */
    var $_internalDownload = false;

    /**
     * Temporary variable used in sorting packages by dependency in {@link sortPkgDeps()}
     * @var array
     * @access private
     */
    var $_packageSortTree;

    /**
     * Temporary directory, or configuration value where downloads will occur
     * @var string
     */
    var $_downloadDir;
    // {{{ PEAR_Downloader()

    /**
     * @param PEAR_Frontend_*
     * @param array
     * @param PEAR_Config
     */
    function PEAR_Downloader(&$ui, $options, &$config)
    {
        parent::PEAR_Common();
        $this->_options = $options;
        $this->config = &$config;
        $this->_preferredState = $this->config->get('preferred_state');
        $this->ui = &$ui;
        if (!$this->_preferredState) {
            // don't inadvertantly use a non-set preferred_state
            $this->_preferredState = null;
        }

        if (isset($this->_options['installroot'])) {
            $this->config->setInstallRoot($this->_options['installroot']);
        }
        $this->_registry = &$config->getRegistry();
        $this->_remote = &$config->getRemote();

        if (isset($this->_options['alldeps']) || isset($this->_options['onlyreqdeps'])) {
            $this->_installed = $this->_registry->listAllPackages();
            foreach ($this->_installed as $key => $unused) {
                if (!count($unused)) {
                    continue;
                }
                @array_walk($this->_installed[$key], 'strtolower');
            }
        }
    }

    /**
     * Attempt to discover a channel's remote capabilities from
     * its server name
     * @param string
     * @return boolean
     */
    function discover($channel)
    {
        $this->log(1, 'Attempting to discover channel "' . $channel . '"...');
        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
        $callback = $this->ui ? array(&$this, '_downloadCallback') : null;
        if (!class_exists('System')) {
            require_once 'System.php';
        }
        $a = $this->downloadHttp('http://' . $channel . '/channel.xml', $this->ui,
            System::mktemp(array('-d')), $callback, false);
        PEAR::popErrorHandling();
        if (PEAR::isError($a)) {
            return false;
        }
        list($a, $lastmodified) = $a;
        if (!class_exists('PEAR/ChannelFile.php')) {
            require_once 'PEAR/ChannelFile.php';
        }
        $b = new PEAR_ChannelFile;
        if ($b->fromXmlFile($a)) {
            @unlink($a);
            if ($this->config->get('auto_discover')) {
                $this->_registry->addChannel($b, $lastmodified);
                $alias = $b->getName();
                if ($b->getName() == $this->_registry->channelName($b->getAlias())) {
                    $alias = $b->getAlias();
                }
                $this->log(1, 'Auto-discovered channel "' . $channel .
                    '", alias "' . $alias . '", adding to registry');
            }
            return true;
        }
        @unlink($a);
        return false;
    }

    /**
     * For simpler unit-testing
     * @param PEAR_Downloader
     * @return PEAR_Downloader_Package
     */
    function &newDownloaderPackage(&$t)
    {
        if (!class_exists('PEAR_Downloader_Package')) {
            require_once 'PEAR/Downloader/Package.php';
        }
        $a = &new PEAR_Downloader_Package($t);
        return $a;
    }

    /**
     * For simpler unit-testing
     * @param PEAR_Config
     * @param array
     * @param array
     * @param int
     */
    function &getDependency2Object(&$c, $i, $p, $s)
    {
        if (!class_exists('PEAR/Dependency2.php')) {
            require_once 'PEAR/Dependency2.php';
        }
        $z = &new PEAR_Dependency2($c, $i, $p, $s);
        return $z;
    }

    function &download($params)
    {
        if (!count($params)) {
            $a = array();
            return $a;
        }
        if (!isset($this->_registry)) {
            $this->_registry = &$this->config->getRegistry();
        }
        if (!isset($this->_remote)) {
            $this->_remote = &$this->config->getRemote();
        }
        $channelschecked = array();
        // convert all parameters into PEAR_Downloader_Package objects
        foreach ($params as $i => $param) {
            $params[$i] = &$this->newDownloaderPackage($this);
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $err = $params[$i]->initialize($param);
            PEAR::staticPopErrorHandling();
            if (!$err) {
                // skip parameters that were missed by preferred_state
                continue;
            }
            if (PEAR::isError($err)) {
                if (!isset($this->_options['soft'])) {
                    $this->log(0, $err->getMessage());
                }
                $params[$i] = false;
                if (is_object($param)) {
                    $param = $param->getChannel() . '/' . $param->getPackage();
                }
                $this->pushError('Package "' . $param . '" is not valid',
                    PEAR_INSTALLER_SKIPPED);
            } else {
                if ($params[$i] && !isset($channelschecked[$params[$i]->getChannel()]) &&
                      !isset($this->_options['offline'])) {
                    $channelschecked[$params[$i]->getChannel()] = true;
                    PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
                    if (!class_exists('System')) {
                        require_once 'System.php';
                    }
                    $curchannel = &$this->_registry->getChannel($params[$i]->getChannel());
                    if (PEAR::isError($curchannel)) {
                        PEAR::staticPopErrorHandling();
                        return $this->raiseError($curchannel);
                    }
                    $a = $this->downloadHttp('http://' . $params[$i]->getChannel() .
                        '/channel.xml', $this->ui,
                        System::mktemp(array('-d')), null, $curchannel->lastModified());
                    PEAR::staticPopErrorHandling();
                    if (PEAR::isError($a) || !$a) {
                        continue;
                    }
                    $this->log(0, 'WARNING: channel "' . $params[$i]->getChannel() . '" has ' .
                        'updated its protocols, use "channel-update ' . $params[$i]->getChannel() .
                        '" to update');
                }
                if ($params[$i] && !isset($this->_options['downloadonly'])) {
                    if (isset($this->_options['packagingroot'])) {
                        $checkdir = $this->_prependPath(
                            $this->config->get('php_dir', null, $params[$i]->getChannel()),
                            $this->_options['packagingroot']);
                    } else {
                        $checkdir = $this->config->get('php_dir',
                            null, $params[$i]->getChannel());
                    }
                    while ($checkdir && $checkdir != '/' && !file_exists($checkdir)) {
                        $checkdir = dirname($checkdir);
                    }
                    if ($checkdir == '.') {
                        $checkdir = '/';
                    }
                    if (!@is_writeable($checkdir)) {
                        return PEAR::raiseError('Cannot install, php_dir for channel "' .
                            $params[$i]->getChannel() . '" is not writeable by the current user');
                    }
                }
            }
        }
        unset($channelschecked);
        PEAR_Downloader_Package::removeDuplicates($params);
        if (!count($params)) {
            $a = array();
            return $a;
        }
        if (!isset($this->_options['nodeps']) && !isset($this->_options['offline'])) {
            $reverify = true;
            while ($reverify) {
                $reverify = false;
                foreach ($params as $i => $param) {
                    $ret = $params[$i]->detectDependencies($params);
                    if (PEAR::isError($ret)) {
                        $reverify = true;
                        $params[$i] = false;
                        PEAR_Downloader_Package::removeDuplicates($params);
                        if (!isset($this->_options['soft'])) {
                            $this->log(0, $ret->getMessage());
                        }
                        continue 2;
                    }
                }
            }
        }
        if (isset($this->_options['offline'])) {
            $this->log(3, 'Skipping dependency download check, --offline specified');
        }
        if (!count($params)) {
            $a = array();
            return $a;
        }
        while (PEAR_Downloader_Package::mergeDependencies($params));
        PEAR_Downloader_Package::removeInstalled($params);
        if (!count($params)) {
            $this->pushError('No valid packages found', PEAR_INSTALLER_FAILED);
            $a = array();
            return $a;
        }
        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
        $err = $this->analyzeDependencies($params);
        PEAR::popErrorHandling();
        if (!count($params)) {
            $this->pushError('No valid packages found', PEAR_INSTALLER_FAILED);
            $a = array();
            return $a;
        }
        $ret = array();
        $newparams = array();
        if (isset($this->_options['pretend'])) {
            return $params;
        }
        foreach ($params as $i => $package) {
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $pf = &$params[$i]->download();
            PEAR::staticPopErrorHandling();
            if (PEAR::isError($pf)) {
                if (!isset($this->_options['soft'])) {
                    $this->log(1, $pf->getMessage());
                    $this->log(0, 'Error: cannot download "' .
                        $this->_registry->parsedPackageNameToString($package->getParsedPackage(),
                            true) .
                        '"');
                }
                continue;
            }
            $newparams[] = &$params[$i];
            $ret[] = array('file' => $pf->getArchiveFile(),
                                   'info' => &$pf,
                                   'pkg' => $pf->getPackage());
        }
        $this->_downloadedPackages = $ret;
        return $newparams;
    }

    /**
     * @param array all packages to be installed
     */
    function analyzeDependencies(&$params)
    {
        $hasfailed = $failed = false;
        if (isset($this->_options['downloadonly'])) {
            return;
        }
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $redo = true;
        $reset = false;
        while ($redo) {
            $redo = false;
            foreach ($params as $i => $param) {
                $deps = $param->getDeps();
                if (!$deps) {
                    $depchecker = &$this->getDependency2Object($this->config, $this->getOptions(),
                        $param->getParsedPackage(), PEAR_VALIDATE_DOWNLOADING);
                    if ($param->getType() == 'xmlrpc') {
                        $send = $param->getDownloadURL();
                    } else {
                        $send = $param->getPackageFile();
                    }
                    $installcheck = $depchecker->validatePackage($send, $this, $params);
                    if (PEAR::isError($installcheck)) {
                        if (!isset($this->_options['soft'])) {
                            $this->log(0, $installcheck->getMessage());
                        }
                        $hasfailed = true;
                        $params[$i] = false;
                        $reset = true;
                        $redo = true;
                        $failed = false;
                        PEAR_Downloader_Package::removeDuplicates($params);
                        continue 2;
                    }
                    continue;
                }
                if (!$reset && $param->alreadyValidated()) {
                    continue;
                }
                if (count($deps)) {
                    $depchecker = &$this->getDependency2Object($this->config, $this->getOptions(),
                        $param->getParsedPackage(), PEAR_VALIDATE_DOWNLOADING);
                    if ($param->getType() == 'xmlrpc') {
                        $send = $param->getDownloadURL();
                    } else {
                        $send = $param->getPackageFile();
                    }
                    $installcheck = $depchecker->validatePackage($send, $this, $params);
                    if (PEAR::isError($installcheck)) {
                        if (!isset($this->_options['soft'])) {
                            $this->log(0, $installcheck->getMessage());
                        }
                        $hasfailed = true;
                        $params[$i] = false;
                        $reset = true;
                        $redo = true;
                        $failed = false;
                        PEAR_Downloader_Package::removeDuplicates($params);
                        continue 2;
                    }
                    $failed = false;
                    if (isset($deps['required'])) {
                        foreach ($deps['required'] as $type => $dep) {
                            // note: Dependency2 will never return a PEAR_Error if ignore-errors
                            // is specified, so soft is needed to turn off logging
                            if (!isset($dep[0])) {
                                if (PEAR::isError($e = $depchecker->{"validate{$type}Dependency"}($dep,
                                      true, $params))) {
                                    $failed = true;
                                    if (!isset($this->_options['soft'])) {
                                        $this->log(0, $e->getMessage());
                                    }
                                } elseif (is_array($e) && !$param->alreadyValidated()) {
                                    if (!isset($this->_options['soft'])) {
                                        $this->log(0, $e[0]);
                                    }
                                }
                            } else {
                                foreach ($dep as $d) {
                                    if (PEAR::isError($e =
                                          $depchecker->{"validate{$type}Dependency"}($d,
                                          true, $params))) {
                                        $failed = true;
                                        if (!isset($this->_options['soft'])) {
                                            $this->log(0, $e->getMessage());
                                        }
                                    } elseif (is_array($e) && !$param->alreadyValidated()) {
                                        if (!isset($this->_options['soft'])) {
                                            $this->log(0, $e[0]);
                                        }
                                    }
                                }
                            }
                        }
                        if (isset($deps['optional'])) {
                            foreach ($deps['optional'] as $type => $dep) {
                                if (!isset($dep[0])) {
                                    if (PEAR::isError($e =
                                          $depchecker->{"validate{$type}Dependency"}($dep,
                                          false, $params))) {
                                        $failed = true;
                                        if (!isset($this->_options['soft'])) {
                                            $this->log(0, $e->getMessage());
                                        }
                                    } elseif (is_array($e) && !$param->alreadyValidated()) {
                                        if (!isset($this->_options['soft'])) {
                                            $this->log(0, $e[0]);
                                        }
                                    }
                                } else {
                                    foreach ($dep as $d) {
                                        if (PEAR::isError($e =
                                              $depchecker->{"validate{$type}Dependency"}($d,
                                              false, $params))) {
                                            $failed = true;
                                            if (!isset($this->_options['soft'])) {
                                                $this->log(0, $e->getMessage());
                                            }
                                        } elseif (is_array($e) && !$param->alreadyValidated()) {
                                            if (!isset($this->_options['soft'])) {
                                                $this->log(0, $e[0]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        $groupname = $param->getGroup();
                        if (isset($deps['group']) && $groupname) {
                            if (!isset($deps['group'][0])) {
                                $deps['group'] = array($deps['group']);
                            }
                            $found = false;
                            foreach ($deps['group'] as $group) {
                                if ($group['attribs']['name'] == $groupname) {
                                    $found = true;
                                    break;
                                }
                            }
                            if ($found) {
                                unset($group['attribs']);
                                foreach ($group as $type => $dep) {
                                    if (!isset($dep[0])) {
                                        if (PEAR::isError($e =
                                              $depchecker->{"validate{$type}Dependency"}($dep,
                                              false, $params))) {
                                            $failed = true;
                                            if (!isset($this->_options['soft'])) {
                                                $this->log(0, $e->getMessage());
                                            }
                                        } elseif (is_array($e) && !$param->alreadyValidated()) {
                                            if (!isset($this->_options['soft'])) {
                                                $this->log(0, $e[0]);
                                            }
                                        }
                                    } else {
                                        foreach ($dep as $d) {
                                            if (PEAR::isError($e =
                                                  $depchecker->{"validate{$type}Dependency"}($d,
                                                  false, $params))) {
                                                $failed = true;
                                                if (!isset($this->_options['soft'])) {
                                                    $this->log(0, $e->getMessage());
                                                }
                                            } elseif (is_array($e) && !$param->alreadyValidated()) {
                                                if (!isset($this->_options['soft'])) {
                                                    $this->log(0, $e[0]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        foreach ($deps as $dep) {
                            if (PEAR::isError($e = $depchecker->validateDependency1($dep, $params))) {
                                $failed = true;
                                if (!isset($this->_options['soft'])) {
                                    $this->log(0, $e->getMessage());
                                }
                            } elseif (is_array($e) && !$param->alreadyValidated()) {
                                if (!isset($this->_options['soft'])) {
                                    $this->log(0, $e[0]);
                                }
                            }
                        }
                    }
                    $params[$i]->setValidated();
                }
                if ($failed) {
                    $hasfailed = true;
                    $params[$i] = false;
                    $reset = true;
                    $redo = true;
                    $failed = false;
                    PEAR_Downloader_Package::removeDuplicates($params);
                    continue 2;
                }
            }
        }
        PEAR::staticPopErrorHandling();
        if ($hasfailed && (isset($this->_options['ignore-errors']) ||
              isset($this->_options['nodeps']))) {
            // this is probably not needed, but just in case
            if (!isset($this->_options['soft'])) {
                $this->log(0, 'WARNING: dependencies failed');
            }
        }
    }

    /**
     * Retrieve the directory that downloads will happen in
     * @access private
     * @return string
     */
    function getDownloadDir()
    {
        if (isset($this->_downloadDir)) {
            return $this->_downloadDir;
        }
        $downloaddir = $this->config->get('download_dir');
        if (empty($downloaddir)) {
            if (!class_exists('System')) {
                require_once 'System.php';
            }
            if (PEAR::isError($downloaddir = System::mktemp('-d'))) {
                return $downloaddir;
            }
            $this->log(3, '+ tmp dir created at ' . $downloaddir);
        }
        return $this->_downloadDir = $downloaddir;
    }

    function setDownloadDir($dir)
    {
        $this->_downloadDir = $dir;
    }

    // }}}
    // {{{ configSet()
    function configSet($key, $value, $layer = 'user', $channel = false)
    {
        $this->config->set($key, $value, $layer, $channel);
        $this->_preferredState = $this->config->get('preferred_state', null, $channel);
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
    // {{{ setOptions()
    function getOptions()
    {
        return $this->_options;
    }

    // }}}

    /**
     * For simpler unit-testing
     * @param PEAR_Config
     * @param int
     * @param string
     */
    function &getPackagefileObject(&$c, $d, $t = false)
    {
        if (!class_exists('PEAR_PackageFile')) {
            require_once 'PEAR/PackageFile.php';
        }
        $a = &new PEAR_PackageFile($c, $d, $t);
        return $a;
    }

    // {{{ _getPackageDownloadUrl()

    /**
     * @param array output of {@link parsePackageName()}
     * @access private
     */
    function _getPackageDownloadUrl($parr)
    {
        $curchannel = $this->config->get('default_channel');
        $this->configSet('default_channel', $parr['channel']);
        // getDownloadURL returns an array.  On error, it only contains information
        // on the latest release as array(version, info).  On success it contains
        // array(version, info, download url string)
        $state = isset($parr['state']) ? $parr['state'] : $this->config->get('preferred_state');
        if (!$this->_registry->channelExists($parr['channel'])) {
            do {
                if ($this->config->get('auto_discover')) {
                    if ($this->discover($parr['channel'])) {
                        break;
                    }
                }
                $this->configSet('default_channel', $curchannel);
                return PEAR::raiseError('Unknown remote channel: ' . $remotechannel);
            } while (false);
        }
        $chan = &$this->_registry->getChannel($parr['channel']);
        if (PEAR::isError($chan)) {
            return $chan;
        }
        $version = $this->_registry->packageInfo($parr['package'], 'version',
            $parr['channel']);
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', $this->_options);
            if (!isset($parr['version']) && !isset($parr['state']) && $version
                  && !isset($this->_options['downloadonly'])) {
                $url = $rest->getDownloadURL($base, $parr, $state, $version);
            } else {
                $url = $rest->getDownloadURL($base, $parr, $state, false);
            }
            if (PEAR::isError($url)) {
                $this->configSet('default_channel', $curchannel);
                return $url;
            }
            if ($parr['channel'] != $curchannel) {
                $this->configSet('default_channel', $curchannel);
            }
            if (!is_array($url)) {
                return $url;
            }
            $url['raw'] = false; // no checking is necessary for REST
            if (!is_array($url['info'])) {
                return PEAR::raiseError('Invalid remote dependencies retrieved from REST - ' .
                    'this should never happen');
            }
            if (isset($url['info']['required']) || $url['compatible']) {
                require_once 'PEAR/PackageFile/v2.php';
                $pf = new PEAR_PackageFile_v2;
                $pf->setRawChannel($parr['channel']);
                if ($url['compatible']) {
                    $pf->setRawCompatible($url['compatible']);
                }
            } else {
                require_once 'PEAR/PackageFile/v1.php';
                $pf = new PEAR_PackageFile_v1;
            }
            $pf->setRawPackage($url['package']);
            $pf->setDeps($url['info']);
            $pf->setRawState($url['stability']);
            $url['info'] = &$pf;
            if (!extension_loaded("zlib") || isset($this->_options['nocompress'])) {
                $ext = '.tar';
            } else {
                $ext = '.tgz';
            }
            if (is_array($url)) {
                if (isset($url['url'])) {
                    $url['url'] .= $ext;
                }
            }
            return $url;
        } elseif ($chan->supports('xmlrpc', 'package.getDownloadURL', false, '1.1')) {
            // don't install with the old version information unless we're doing a plain
            // vanilla simple installation.  If the user says to install a particular
            // version or state, ignore the current installed version
            if (!isset($parr['version']) && !isset($parr['state']) && $version
                  && !isset($this->_options['downloadonly'])) {
                $url = $this->_remote->call('package.getDownloadURL', $parr, $state, $version);
            } else {
                $url = $this->_remote->call('package.getDownloadURL', $parr, $state);
            }
        } else {
            $url = $this->_remote->call('package.getDownloadURL', $parr, $state);
        }
        if (PEAR::isError($url)) {
            return $url;
        }
        if ($parr['channel'] != $curchannel) {
            $this->configSet('default_channel', $curchannel);
        }
        if (isset($url['__PEAR_ERROR_CLASS__'])) {
            return PEAR::raiseError($url['message']);
        }
        if (!is_array($url)) {
            return $url;
        }
        $url['raw'] = $url['info'];
        if (isset($this->_options['downloadonly'])) {
            $pkg = &$this->getPackagefileObject($this->config, $this->debug);
        } else {
            $pkg = &$this->getPackagefileObject($this->config, $this->debug,
                $this->getDownloadDir());
        }
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $pinfo = &$pkg->fromXmlString($url['info'], PEAR_VALIDATE_DOWNLOADING, 'remote');
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($pinfo)) {
            if (!isset($this->_options['soft'])) {
                $this->log(0, $pinfo->getMessage());
            }
            return PEAR::raiseError('Remote package.xml is not valid - this should never happen');
        }
        $url['info'] = &$pinfo;
        if (!extension_loaded("zlib") || isset($this->_options['nocompress'])) {
            $ext = '.tar';
        } else {
            $ext = '.tgz';
        }
        if (is_array($url)) {
            if (isset($url['url'])) {
                $url['url'] .= $ext;
            }
        }
        return $url;
    }
    // }}}
    // {{{ getDepPackageDownloadUrl()

    /**
     * @param array dependency array
     * @access private
     */
    function _getDepPackageDownloadUrl($dep, $parr)
    {
        $xsdversion = isset($dep['rel']) ? '1.0' : '2.0';
        $curchannel = $this->config->get('default_channel');
        if (isset($dep['channel'])) {
            $remotechannel = $dep['channel'];
        } else {
            $remotechannel = 'pear.php.net';
        }
        if (!$this->_registry->channelExists($remotechannel)) {
            do {
                if ($this->config->get('auto_discover')) {
                    if ($this->discover($remotechannel)) {
                        break;
                    }
                }
                return PEAR::raiseError('Unknown remote channel: ' . $remotechannel);
            } while (false);
        }
        $this->configSet('default_channel', $remotechannel);
        $state = isset($parr['state']) ? $parr['state'] : $this->config->get('preferred_state');
        if (isset($parr['state']) && isset($parr['version'])) {
            unset($parr['state']);
        }
        $chan = &$this->_registry->getChannel($remotechannel);
        if (PEAR::isError($chan)) {
            return $chan;
        }
        $version = $this->_registry->packageInfo($dep['name'], 'version',
            $remotechannel);
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', $this->_options);
            $url = $rest->getDepDownloadURL($base, $xsdversion, $dep, $parr,
                    $state, $version);
            if (PEAR::isError($url)) {
                return $url;
            }
            if ($parr['channel'] != $curchannel) {
                $this->configSet('default_channel', $curchannel);
            }
            if (!is_array($url)) {
                return $url;
            }
            $url['raw'] = false; // no checking is necessary for REST
            if (!is_array($url['info'])) {
                return PEAR::raiseError('Invalid remote dependencies retrieved from REST - ' .
                    'this should never happen');
            }
            if (isset($url['info']['required'])) {
                if (!class_exists('PEAR_PackageFile_v2')) {
                    require_once 'PEAR/PackageFile/v2.php';
                }
                $pf = new PEAR_PackageFile_v2;
                $pf->setRawChannel($remotechannel);
            } else {
                if (!class_exists('PEAR_PackageFile_v1')) {
                    require_once 'PEAR/PackageFile/v1.php';
                }
                $pf = new PEAR_PackageFile_v1;
            }
            $pf->setRawPackage($url['package']);
            $pf->setDeps($url['info']);
            $pf->setRawState($url['stability']);
            $url['info'] = &$pf;
            if (!extension_loaded("zlib") || isset($this->_options['nocompress'])) {
                $ext = '.tar';
            } else {
                $ext = '.tgz';
            }
            if (is_array($url)) {
                if (isset($url['url'])) {
                    $url['url'] .= $ext;
                }
            }
            return $url;
        } elseif ($chan->supports('xmlrpc', 'package.getDepDownloadURL', false, '1.1')) {
            if ($version) {
                $url = $this->_remote->call('package.getDepDownloadURL', $xsdversion, $dep, $parr,
                    $state, $version);
            } else {
                $url = $this->_remote->call('package.getDepDownloadURL', $xsdversion, $dep, $parr,
                    $state);
            }
        } else {
            $url = $this->_remote->call('package.getDepDownloadURL', $xsdversion, $dep, $parr, $state);
        }
        if ($parr['channel'] != $curchannel) {
            $this->configSet('default_channel', $curchannel);
        }
        if (!is_array($url)) {
            return $url;
        }
        if (isset($url['__PEAR_ERROR_CLASS__'])) {
            return PEAR::raiseError($url['message']);
        }
        $url['raw'] = $url['info'];
        $pkg = &$this->getPackagefileObject($this->config, $this->debug);
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $pinfo = &$pkg->fromXmlString($url['info'], PEAR_VALIDATE_DOWNLOADING, 'remote');
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($pinfo)) {
            if (!isset($this->_options['soft'])) {
                $this->log(0, $pinfo->getMessage());
            }
            return PEAR::raiseError('Remote package.xml is not valid - this should never happen');
        }
        $url['info'] = &$pinfo;
        if (is_array($url)) {
            if (!extension_loaded("zlib") || isset($this->_options['nocompress'])) {
                $ext = '.tar';
            } else {
                $ext = '.tgz';
            }
            if (isset($url['url'])) {
                $url['url'] .= $ext;
            }
        }
        return $url;
    }
    // }}}
    // {{{ getPackageDownloadUrl()

    /**
     * @deprecated in favor of _getPackageDownloadUrl
     */
    function getPackageDownloadUrl($package, $version = null, $channel = false)
    {
        if ($version) {
            $package .= "-$version";
        }
        if ($this === null || $this->_registry === null) {
            $package = "http://pear.php.net/get/$package";
        } else {
            $chan = $this->_registry->getChannel($channel);
            if (PEAR::isError($chan)) {
                return '';
            }
            $package = "http://" . $chan->getServer() . "/get/$package";
        }
        if (!extension_loaded("zlib")) {
            $package .= '?uncompress=yes';
        }
        return $package;
    }

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
                if (preg_match('/^[a-z]:/i', $prepend)) {
                    $prepend = substr($prepend, 2);
                } elseif ($prepend{0} != '\\') {
                    $prepend = "\\$prepend";
                }
                $path = substr($path, 0, 2) . $prepend . substr($path, 2);
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

    /**
     * for BC
     */
    function sortPkgDeps(&$packages, $uninstall = false)
    {
        $uninstall ? 
            $this->sortPackagesForUninstall($packages) :
            $this->sortPackagesForInstall($packages);
    }

    function _getDepTreeDP($package, $packages, &$deps, &$checked)
    {
        $pf = $package->getPackageFile();
        $checked[strtolower($package->getChannel())][strtolower($package->getPackage())]
            = true;
        $pdeps = $pf->getDeps(true);
        if (!$pdeps) {
            return;
        }
        if ($pf->getPackagexmlVersion() == '1.0') {
            foreach ($pdeps as $dep) {
                if ($dep['type'] != 'pkg') {
                    continue;
                }
                $deps['pear.php.net'][strtolower($dep['name'])] = true;
                foreach ($packages as $p) {
                    $dep['channel'] = 'pear.php.net';
                    $dep['package'] = $dep['name'];
                    if ($p->isEqual($dep)) {
                        if (!isset($checked[strtolower($p->getChannel())]
                              [strtolower($p->getPackage())])) {
                            // add the dependency's dependencies to the tree
                            $this->_getDepTreeDP($p, $packages, $deps, $checked);
                        }
                    }
                }
            }
        } else {
            $tdeps = array();
            if (isset($pdeps['required']['package'])) {
                $t = $pdeps['required']['package'];
                if (!isset($t[0])) {
                    $t = array($t);
                }
                $tdeps = array_merge($tdeps, $t);
            }
            if (isset($pdeps['required']['subpackage'])) {
                $t = $pdeps['required']['subpackage'];
                if (!isset($t[0])) {
                    $t = array($t);
                }
                $tdeps = array_merge($tdeps, $t);
            }
            if (isset($pdeps['optional']['package'])) {
                $t = $pdeps['optional']['package'];
                if (!isset($t[0])) {
                    $t = array($t);
                }
                $tdeps = array_merge($tdeps, $t);
            }
            if (isset($pdeps['optional']['subpackage'])) {
                $t = $pdeps['optional']['subpackage'];
                if (!isset($t[0])) {
                    $t = array($t);
                }
                $tdeps = array_merge($tdeps, $t);
            }
            if (isset($pdeps['group'])) {
                if (!isset($pdeps['group'][0])) {
                    $pdeps['group'] = array($pdeps['group']);
                }
                foreach ($pdeps['group'] as $group) {
                    if (isset($group['package'])) {
                        $t = $group['package'];
                        if (!isset($t[0])) {
                            $t = array($t);
                        }
                        $tdeps = array_merge($tdeps, $t);
                    }
                    if (isset($group['subpackage'])) {
                        $t = $group['subpackage'];
                        if (!isset($t[0])) {
                            $t = array($t);
                        }
                        $tdeps = array_merge($tdeps, $t);
                    }
                }
            }
            foreach ($tdeps as $dep) {
                if (!isset($dep['channel'])) {
                    $depchannel = '__uri';
                } else {
                    $depchannel = $dep['channel'];
                }
                $deps[$depchannel][strtolower($dep['name'])] = true;
                foreach ($packages as $p) {
                    $dep['channel'] = $depchannel;
                    $dep['package'] = $dep['name'];
                    if ($p->isEqual($dep)) {
                        if (!isset($checked[strtolower($p->getChannel())]
                              [strtolower($p->getPackage())])) {
                            // add the dependency's dependencies to the tree
                            $this->_getDepTreeDP($p, $packages, $deps, $checked);
                        }
                    }
                }
            }
        }
    }

    /**
     * Sort a list of arrays of array(downloaded packagefilename) by dependency.
     *
     * It also removes duplicate dependencies
     * @param array an array of downloaded PEAR_Downloader_Packages
     * @return array array of array(packagefilename, package.xml contents)
     */
    function sortPackagesForInstall(&$packages)
    {
        foreach ($packages as $i => $package) {
            $checked = $deps = array();
            $this->_getDepTreeDP($packages[$i], $packages, $deps, $checked);
            $this->_depTree[$package->getChannel()][$package->getPackage()] = $deps;
        }
        usort($packages, array(&$this, '_sortInstall'));
    }

    function _dependsOn($a, $b)
    {
        return $this->_checkDepTree(strtolower($a->getChannel()), strtolower($a->getPackage()),
            $b);
    }

    function _checkDepTree($channel, $package, $b, $checked = array())
    {
        $checked[$channel][$package] = true;
        if (!isset($this->_depTree[$channel][$package])) {
            return false;
        }
        if (isset($this->_depTree[$channel][$package][strtolower($b->getChannel())]
              [strtolower($b->getPackage())])) {
            return true;
        }
        foreach ($this->_depTree[$channel][$package] as $ch => $packages) {
            foreach ($packages as $pa => $true) {
                if ($this->_checkDepTree($ch, $pa, $b, $checked)) {
                    return true;
                }
            }
        }
        return false;
    }

    function _sortInstall($a, $b)
    {
        if (!$a->getDeps() && !$b->getDeps()) {
            return 0; // neither package has dependencies, order is insignificant
        }
        if ($a->getDeps() && !$b->getDeps()) {
            return 1; // $a must be installed after $b because $a has dependencies
        }
        if (!$a->getDeps() && $b->getDeps()) {
            return -1; // $b must be installed after $a because $b has dependencies
        }
        // both packages have dependencies
        if ($this->_dependsOn($a, $b)) {
            return 1;
        }
        if ($this->_dependsOn($b, $a)) {
            return -1;
        }
        return 0;
    }

    /**
     * Download a file through HTTP.  Considers suggested file name in
     * Content-disposition: header and can run a callback function for
     * different events.  The callback will be called with two
     * parameters: the callback type, and parameters.  The implemented
     * callback types are:
     *
     *  'setup'       called at the very beginning, parameter is a UI object
     *                that should be used for all output
     *  'message'     the parameter is a string with an informational message
     *  'saveas'      may be used to save with a different file name, the
     *                parameter is the filename that is about to be used.
     *                If a 'saveas' callback returns a non-empty string,
     *                that file name will be used as the filename instead.
     *                Note that $save_dir will not be affected by this, only
     *                the basename of the file.
     *  'start'       download is starting, parameter is number of bytes
     *                that are expected, or -1 if unknown
     *  'bytesread'   parameter is the number of bytes read so far
     *  'done'        download is complete, parameter is the total number
     *                of bytes read
     *  'connfailed'  if the TCP/SSL connection fails, this callback is called
     *                with array(host,port,errno,errmsg)
     *  'writefailed' if writing to disk fails, this callback is called
     *                with array(destfile,errmsg)
     *
     * If an HTTP proxy has been configured (http_proxy PEAR_Config
     * setting), the proxy will be used.
     *
     * @param string  $url       the URL to download
     * @param object  $ui        PEAR_Frontend_* instance
     * @param object  $config    PEAR_Config instance
     * @param string  $save_dir  directory to save file in
     * @param mixed   $callback  function/method to call for status
     *                           updates
     * @param false|string|array $lastmodified header values to check against for caching
     *                           use false to return the header values from this download
     * @param false|array $accept Accept headers to send
     * @return string|array  Returns the full path of the downloaded file or a PEAR
     *                       error on failure.  If the error is caused by
     *                       socket-related errors, the error object will
     *                       have the fsockopen error code available through
     *                       getCode().  If caching is requested, then return the header
     *                       values.
     *
     * @access public
     */
    function downloadHttp($url, &$ui, $save_dir = '.', $callback = null, $lastmodified = null,
                          $accept = false)
    {
        static $redirect = 0;
        // allways reset , so we are clean case of error
        $wasredirect = $redirect;
        $redirect = 0;
        if ($callback) {
            call_user_func($callback, 'setup', array(&$ui));
        }
        $info = parse_url($url);
        if (!isset($info['scheme']) || !in_array($info['scheme'], array('http', 'https'))) {
            return PEAR::raiseError('Cannot download non-http URL "' . $url . '"');
        }
        if (!isset($info['host'])) {
            return PEAR::raiseError('Cannot download from non-URL "' . $url . '"');
        } else {
            $host = @$info['host'];
            $port = @$info['port'];
            $path = @$info['path'];
        }
        if (isset($this)) {
            $config = &$this->config;
        } else {
            $config = &PEAR_Config::singleton();
        }
        $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
        if ($config->get('http_proxy')&& 
              $proxy = parse_url($config->get('http_proxy'))) {
            $proxy_host = @$proxy['host'];
            if (isset($proxy['scheme']) && $proxy['scheme'] == 'https') {
                $proxy_host = 'ssl://' . $proxy_host;
            }
            $proxy_port = @$proxy['port'];
            $proxy_user = @$proxy['user'];
            $proxy_pass = @$proxy['pass'];

            if ($proxy_port == '') {
                $proxy_port = 8080;
            }
            if ($callback) {
                call_user_func($callback, 'message', "Using HTTP proxy $host:$port");
            }
        }
        if (empty($port)) {
            if (isset($info['scheme']) && $info['scheme'] == 'https') {
                $port = 443;
            } else {
                $port = 80;
            }
        }
        if ($proxy_host != '') {
            $fp = @fsockopen($proxy_host, $proxy_port, $errno, $errstr);
            if (!$fp) {
                if ($callback) {
                    call_user_func($callback, 'connfailed', array($proxy_host, $proxy_port,
                                                                  $errno, $errstr));
                }
                return PEAR::raiseError("Connection to `$proxy_host:$proxy_port' failed: $errstr", $errno);
            }
            if ($lastmodified === false || $lastmodified) {
                $request = "GET $url HTTP/1.1\r\n";
            } else {
                $request = "GET $url HTTP/1.0\r\n";
            }
        } else {
            if (isset($info['scheme']) && $info['scheme'] == 'https') {
                $host = 'ssl://' . $host;
            }
            $fp = @fsockopen($host, $port, $errno, $errstr);
            if (!$fp) {
                if ($callback) {
                    call_user_func($callback, 'connfailed', array($host, $port,
                                                                  $errno, $errstr));
                }
                return PEAR::raiseError("Connection to `$host:$port' failed: $errstr", $errno);
            }
            if ($lastmodified === false || $lastmodified) {
                $request = "GET $path HTTP/1.1\r\n";
            } else {
                $request = "GET $path HTTP/1.0\r\n";
            }
        }
        $ifmodifiedsince = '';
        if (is_array($lastmodified)) {
            if (isset($lastmodified['Last-Modified'])) {
                $ifmodifiedsince = 'If-Modified-Since: ' . $lastmodified['Last-Modified'] . "\r\n";
            }
            if (isset($lastmodified['ETag'])) {
                $ifmodifiedsince .= "If-None-Match: $lastmodified[ETag]\r\n";
            }
        } else {
            $ifmodifiedsince = ($lastmodified ? "If-Modified-Since: $lastmodified\r\n" : '');
        }
        $request .= "Host: $host:$port\r\n" . $ifmodifiedsince .
            "User-Agent: PEAR/1.4.9/PHP/" . PHP_VERSION . "\r\n";
        if (isset($this)) { // only pass in authentication for non-static calls
            $username = $config->get('username');
            $password = $config->get('password');
            if ($username && $password) {
                $tmp = base64_encode("$username:$password");
                $request .= "Authorization: Basic $tmp\r\n";
            }
        }
        if ($proxy_host != '' && $proxy_user != '') {
            $request .= 'Proxy-Authorization: Basic ' .
                base64_encode($proxy_user . ':' . $proxy_pass) . "\r\n";
        }
        if ($accept) {
            $request .= 'Accept: ' . implode(', ', $accept) . "\r\n";
        }
        $request .= "Connection: close\r\n";
        $request .= "\r\n";
        fwrite($fp, $request);
        $headers = array();
        $reply = 0;
        while (trim($line = fgets($fp, 1024))) {
            if (preg_match('/^([^:]+):\s+(.*)\s*$/', $line, $matches)) {
                $headers[strtolower($matches[1])] = trim($matches[2]);
            } elseif (preg_match('|^HTTP/1.[01] ([0-9]{3}) |', $line, $matches)) {
                $reply = (int) $matches[1];
                if ($reply == 304 && ($lastmodified || ($lastmodified === false))) {
                    return false;
                }
                if (! in_array($reply, array(200, 301, 302, 303, 305, 307))) {
                    return PEAR::raiseError("File http://$host:$port$path not valid (received: $line)");
                }
            }
        }
        if ($reply != 200) {
            if (isset($headers['location'])) {
                if ($wasredirect < 5) {
                    $redirect = $wasredirect + 1;
                    return $this->downloadHttp($headers['location'],
                            $ui, $save_dir, $callback, $lastmodified, $accept);
                } else {
                    return PEAR::raiseError("File http://$host:$port$path not valid (redirection looped more than 5 times)");
                }
            } else {
                return PEAR::raiseError("File http://$host:$port$path not valid (redirected but no location)");
            }
        }
        if (isset($headers['content-disposition']) &&
            preg_match('/\sfilename=\"([^;]*\S)\"\s*(;|$)/', $headers['content-disposition'], $matches)) {
            $save_as = basename($matches[1]);
        } else {
            $save_as = basename($url);
        }
        if ($callback) {
            $tmp = call_user_func($callback, 'saveas', $save_as);
            if ($tmp) {
                $save_as = $tmp;
            }
        }
        $dest_file = $save_dir . DIRECTORY_SEPARATOR . $save_as;
        if (!$wp = @fopen($dest_file, 'wb')) {
            fclose($fp);
            if ($callback) {
                call_user_func($callback, 'writefailed', array($dest_file, $php_errormsg));
            }
            return PEAR::raiseError("could not open $dest_file for writing");
        }
        if (isset($headers['content-length'])) {
            $length = $headers['content-length'];
        } else {
            $length = -1;
        }
        $bytes = 0;
        if ($callback) {
            call_user_func($callback, 'start', array(basename($dest_file), $length));
        }
        while ($data = @fread($fp, 1024)) {
            $bytes += strlen($data);
            if ($callback) {
                call_user_func($callback, 'bytesread', $bytes);
            }
            if (!@fwrite($wp, $data)) {
                fclose($fp);
                if ($callback) {
                    call_user_func($callback, 'writefailed', array($dest_file, $php_errormsg));
                }
                return PEAR::raiseError("$dest_file: write failed ($php_errormsg)");
            }
        }
        fclose($fp);
        fclose($wp);
        if ($callback) {
            call_user_func($callback, 'done', $bytes);
        }
        if ($lastmodified === false || $lastmodified) {
            if (isset($headers['etag'])) {
                $lastmodified = array('ETag' => $headers['etag']);
            }
            if (isset($headers['last-modified'])) {
                if (is_array($lastmodified)) {
                    $lastmodified['Last-Modified'] = $headers['last-modified'];
                } else {
                    $lastmodified = $headers['last-modified'];
                }
            }
            return array($dest_file, $lastmodified, $headers);
        }
        return $dest_file;
    }
}
// }}}

?>
