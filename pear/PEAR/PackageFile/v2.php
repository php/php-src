<?php
/**
 * PEAR_PackageFile_v2, package.xml version 2.0
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
 * For error handling
 */
require_once 'PEAR/ErrorStack.php';
/**
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_PackageFile_v2
{

    /**
     * Parsed package information
     * @var array
     * @access private
     */
    var $_packageInfo = array();

    /**
     * path to package .tgz or false if this is a local/extracted package.xml
     * @var string|false
     * @access private
     */
    var $_archiveFile;

    /**
     * path to package .xml or false if this is an abstract parsed-from-string xml
     * @var string|false
     * @access private
     */
    var $_packageFile;

    /**
     * This is used by file analysis routines to log progress information
     * @var PEAR_Common
     * @access protected
     */
    var $_logger;

    /**
     * This is set to the highest validation level that has been validated
     *
     * If the package.xml is invalid or unknown, this is set to 0.  If
     * normal validation has occurred, this is set to PEAR_VALIDATE_NORMAL.  If
     * downloading/installation validation has occurred it is set to PEAR_VALIDATE_DOWNLOADING
     * or INSTALLING, and so on up to PEAR_VALIDATE_PACKAGING.  This allows validation
     * "caching" to occur, which is particularly important for package validation, so
     * that PHP files are not validated twice
     * @var int
     * @access private
     */
    var $_isValid = 0;

    /**
     * True if the filelist has been validated
     * @param bool
     */
    var $_filesValid = false;

    /**
     * @var PEAR_Registry
     * @access protected
     */
    var $_registry;

    /**
     * @var PEAR_Config
     * @access protected
     */
    var $_config;

    /**
     * Optional Dependency group requested for installation
     * @var string
     * @access private
     */
    var $_requestedGroup = false;

    /**
     * @var PEAR_ErrorStack
     * @access protected
     */
    var $_stack;

    /**
     * Namespace prefix used for tasks in this package.xml - use tasks: whenever possible
     */
    var $_tasksNs;

    /**
     * Determines whether this packagefile was initialized only with partial package info
     *
     * If this package file was constructed via parsing REST, it will only contain
     *
     * - package name
     * - channel name
     * - dependencies 
     * @var boolean
     * @access private
     */
    var $_incomplete = true;

    /**
     * @var PEAR_PackageFile_v2_Validator
     */
    var $_v2Validator;

    /**
     * The constructor merely sets up the private error stack
     */
    function PEAR_PackageFile_v2()
    {
        $this->_stack = new PEAR_ErrorStack('PEAR_PackageFile_v2', false, null);
        $this->_isValid = false;
    }

    /**
     * To make unit-testing easier
     * @param PEAR_Frontend_*
     * @param array options
     * @param PEAR_Config
     * @return PEAR_Downloader
     * @access protected
     */
    function &getPEARDownloader(&$i, $o, &$c)
    {
        $z = &new PEAR_Downloader($i, $o, $c);
        return $z;
    }

    /**
     * To make unit-testing easier
     * @param PEAR_Config
     * @param array options
     * @param array package name as returned from {@link PEAR_Registry::parsePackageName()}
     * @param int PEAR_VALIDATE_* constant
     * @return PEAR_Dependency2
     * @access protected
     */
    function &getPEARDependency2(&$c, $o, $p, $s = PEAR_VALIDATE_INSTALLING)
    {
        if (!class_exists('PEAR_Dependency2')) {
            require_once 'PEAR/Dependency2.php';
        }
        $z = &new PEAR_Dependency2($c, $o, $p, $s);
        return $z;
    }

    function getInstalledBinary()
    {
        return isset($this->_packageInfo['#binarypackage']) ? $this->_packageInfo['#binarypackage'] :
            false;
    }

    /**
     * Installation of source package has failed, attempt to download and install the
     * binary version of this package.
     * @param PEAR_Installer
     * @return array|false
     */
    function installBinary(&$installer)
    {
        if (!OS_WINDOWS) {
            $a = false;
            return $a;
        }
        if ($this->getPackageType() == 'extsrc') {
            if (!is_array($installer->getInstallPackages())) {
                $a = false;
                return $a;
            }
            foreach ($installer->getInstallPackages() as $p) {
                if ($p->isExtension($this->_packageInfo['providesextension'])) {
                    if ($p->getPackageType() != 'extsrc') {
                        $a = false;
                        return $a; // the user probably downloaded it separately
                    }
                }
            }
            if (isset($this->_packageInfo['extsrcrelease']['binarypackage'])) {
                $installer->log(0, 'Attempting to download binary version of extension "' .
                    $this->_packageInfo['providesextension'] . '"');
                $params = $this->_packageInfo['extsrcrelease']['binarypackage'];
                if (!is_array($params) || !isset($params[0])) {
                    $params = array($params);
                }
                if (isset($this->_packageInfo['channel'])) {
                    foreach ($params as $i => $param) {
                        $params[$i] = array('channel' => $this->_packageInfo['channel'],
                            'package' => $param, 'version' => $this->getVersion());
                    }
                }
                $dl = &$this->getPEARDownloader($installer->ui, $installer->getOptions(),
                    $installer->config);
                $verbose = $dl->config->get('verbose');
                $dl->config->set('verbose', -1);
                foreach ($params as $param) {
                    PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
                    $ret = $dl->download(array($param));
                    PEAR::popErrorHandling();
                    if (is_array($ret) && count($ret)) {
                        break;
                    }
                }
                $dl->config->set('verbose', $verbose);
                if (is_array($ret)) {
                    if (count($ret) == 1) {
                        $pf = $ret[0]->getPackageFile();
                        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
                        $err = $installer->install($ret[0]);
                        PEAR::popErrorHandling();
                        if (is_array($err)) {
                            $this->_packageInfo['#binarypackage'] = $ret[0]->getPackage();
                            // "install" self, so all dependencies will work transparently
                            $this->_registry->addPackage2($this);
                            $installer->log(0, 'Download and install of binary extension "' .
                                $this->_registry->parsedPackageNameToString(
                                    array('channel' => $pf->getChannel(),
                                          'package' => $pf->getPackage()), true) . '" successful');
                            $a = array($ret[0], $err);
                            return $a;
                        }
                        $installer->log(0, 'Download and install of binary extension "' .
                            $this->_registry->parsedPackageNameToString(
                                    array('channel' => $pf->getChannel(),
                                          'package' => $pf->getPackage()), true) . '" failed');
                    }
                }
            }
        }
        $a = false;
        return $a;
    }

    /**
     * @return string|false Extension name
     */
    function getProvidesExtension()
    {
        if (in_array($this->getPackageType(), array('extsrc', 'extbin'))) {
            if (isset($this->_packageInfo['providesextension'])) {
                return $this->_packageInfo['providesextension'];
            }
        }
        return false;
    }

    /**
     * @param string Extension name
     * @return bool
     */
    function isExtension($extension)
    {
        if (in_array($this->getPackageType(), array('extsrc', 'extbin'))) {
            return $this->_packageInfo['providesextension'] == $extension;
        }
        return false;
    }

    /**
     * Tests whether every part of the package.xml 1.0 is represented in
     * this package.xml 2.0
     * @param PEAR_PackageFile_v1
     * @return bool
     */
    function isEquivalent($pf1)
    {
        if (!$pf1) {
            return true;
        }
        if ($this->getPackageType() == 'bundle') {
            return false;
        }
        $this->_stack->getErrors(true);
        if (!$pf1->validate(PEAR_VALIDATE_NORMAL)) {
            return false;
        }
        $pass = true;
        if ($pf1->getPackage() != $this->getPackage()) {
            $this->_differentPackage($pf1->getPackage());
            $pass = false;
        }
        if ($pf1->getVersion() != $this->getVersion()) {
            $this->_differentVersion($pf1->getVersion());
            $pass = false;
        }
        if (trim($pf1->getSummary()) != $this->getSummary()) {
            $this->_differentSummary($pf1->getSummary());
            $pass = false;
        }
        if (preg_replace('/\s+/', '', $pf1->getDescription()) !=
              preg_replace('/\s+/', '', $this->getDescription())) {
            $this->_differentDescription($pf1->getDescription());
            $pass = false;
        }
        if ($pf1->getState() != $this->getState()) {
            $this->_differentState($pf1->getState());
            $pass = false;
        }
        if (!strstr(preg_replace('/\s+/', '', $this->getNotes()),
              preg_replace('/\s+/', '', $pf1->getNotes()))) {
            $this->_differentNotes($pf1->getNotes());
            $pass = false;
        }
        $mymaintainers = $this->getMaintainers();
        $yourmaintainers = $pf1->getMaintainers();
        for ($i1 = 0; $i1 < count($yourmaintainers); $i1++) {
            $reset = false;
            for ($i2 = 0; $i2 < count($mymaintainers); $i2++) {
                if ($mymaintainers[$i2]['handle'] == $yourmaintainers[$i1]['handle']) {
                    if ($mymaintainers[$i2]['role'] != $yourmaintainers[$i1]['role']) {
                        $this->_differentRole($mymaintainers[$i2]['handle'],
                            $yourmaintainers[$i1]['role'], $mymaintainers[$i2]['role']);
                        $pass = false;
                    }
                    if ($mymaintainers[$i2]['email'] != $yourmaintainers[$i1]['email']) {
                        $this->_differentEmail($mymaintainers[$i2]['handle'],
                            $yourmaintainers[$i1]['email'], $mymaintainers[$i2]['email']);
                        $pass = false;
                    }
                    if ($mymaintainers[$i2]['name'] != $yourmaintainers[$i1]['name']) {
                        $this->_differentName($mymaintainers[$i2]['handle'],
                            $yourmaintainers[$i1]['name'], $mymaintainers[$i2]['name']);
                        $pass = false;
                    }
                    unset($mymaintainers[$i2]);
                    $mymaintainers = array_values($mymaintainers);
                    unset($yourmaintainers[$i1]);
                    $yourmaintainers = array_values($yourmaintainers);
                    $reset = true;
                    break;
                }
            }
            if ($reset) {
                $i1 = -1;
            }
        }
        $this->_unmatchedMaintainers($mymaintainers, $yourmaintainers);
        $filelist = $this->getFilelist();
        foreach ($pf1->getFilelist() as $file => $atts) {
            if (!isset($filelist[$file])) {
                $this->_missingFile($file);
                $pass = false;
            }
        }
        return $pass;
    }

    function _differentPackage($package)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('package' => $package,
            'self' => $this->getPackage()),
            'package.xml 1.0 package "%package%" does not match "%self%"');
    }

    function _differentVersion($version)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('version' => $version,
            'self' => $this->getVersion()),
            'package.xml 1.0 version "%version%" does not match "%self%"');
    }

    function _differentState($state)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('state' => $state,
            'self' => $this->getState()),
            'package.xml 1.0 state "%state%" does not match "%self%"');
    }

    function _differentRole($handle, $role, $selfrole)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('handle' => $handle,
            'role' => $role, 'self' => $selfrole),
            'package.xml 1.0 maintainer "%handle%" role "%role%" does not match "%self%"');
    }

    function _differentEmail($handle, $email, $selfemail)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('handle' => $handle,
            'email' => $email, 'self' => $selfemail),
            'package.xml 1.0 maintainer "%handle%" email "%email%" does not match "%self%"');
    }

    function _differentName($handle, $name, $selfname)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('handle' => $handle,
            'name' => $name, 'self' => $selfname),
            'package.xml 1.0 maintainer "%handle%" name "%name%" does not match "%self%"');
    }

    function _unmatchedMaintainers($my, $yours)
    {
        if ($my) {
            array_walk($my, create_function('&$i, $k', '$i = $i["handle"];'));
            $this->_stack->push(__FUNCTION__, 'error', array('handles' => $my),
                'package.xml 2.0 has unmatched extra maintainers "%handles%"');
        }
        if ($yours) {
            array_walk($yours, create_function('&$i, $k', '$i = $i["handle"];'));
            $this->_stack->push(__FUNCTION__, 'error', array('handles' => $yours),
                'package.xml 1.0 has unmatched extra maintainers "%handles%"');
        }
    }

    function _differentNotes($notes)
    {
        $truncnotes = strlen($notes) < 25 ? $notes : substr($notes, 0, 24) . '...';
        $truncmynotes = strlen($this->getNotes()) < 25 ? $this->getNotes() :
            substr($this->getNotes(), 0, 24) . '...';
        $this->_stack->push(__FUNCTION__, 'error', array('notes' => $truncnotes,
            'self' => $truncmynotes),
            'package.xml 1.0 release notes "%notes%" do not match "%self%"');
    }

    function _differentSummary($summary)
    {
        $truncsummary = strlen($summary) < 25 ? $summary : substr($summary, 0, 24) . '...';
        $truncmysummary = strlen($this->getsummary()) < 25 ? $this->getSummary() :
            substr($this->getsummary(), 0, 24) . '...';
        $this->_stack->push(__FUNCTION__, 'error', array('summary' => $truncsummary,
            'self' => $truncmysummary),
            'package.xml 1.0 summary "%summary%" does not match "%self%"');
    }

    function _differentDescription($description)
    {
        $truncdescription = trim(strlen($description) < 25 ? $description : substr($description, 0, 24) . '...');
        $truncmydescription = trim(strlen($this->getDescription()) < 25 ? $this->getDescription() :
            substr($this->getdescription(), 0, 24) . '...');
        $this->_stack->push(__FUNCTION__, 'error', array('description' => $truncdescription,
            'self' => $truncmydescription),
            'package.xml 1.0 description "%description%" does not match "%self%"');
    }

    function _missingFile($file)
    {
        $this->_stack->push(__FUNCTION__, 'error', array('file' => $file),
            'package.xml 1.0 file "%file%" is not present in <contents>');
    }

    /**
     * WARNING - do not use this function unless you know what you're doing
     */
    function setRawState($state)
    {
        $this->_packageInfo['stability']['release'] = $state;
    }

    /**
     * WARNING - do not use this function unless you know what you're doing
     */
    function setRawCompatible($compatible)
    {
        $this->_packageInfo['compatible'] = $compatible;
    }

    /**
     * WARNING - do not use this function unless you know what you're doing
     */
    function setRawPackage($package)
    {
        $this->_packageInfo['name'] = $package;
    }

    /**
     * WARNING - do not use this function unless you know what you're doing
     */
    function setRawChannel($channel)
    {
        $this->_packageInfo['channel'] = $channel;
    }

    function setRequestedGroup($group)
    {
        $this->_requestedGroup = $group;
    }

    function getRequestedGroup()
    {
        if (isset($this->_requestedGroup)) {
            return $this->_requestedGroup;
        }
        return false;
    }

    /**
     * For saving in the registry.
     *
     * Set the last version that was installed
     * @param string
     */
    function setLastInstalledVersion($version)
    {
        $this->_packageInfo['_lastversion'] = $version;
    }

    /**
     * @return string|false
     */
    function getLastInstalledVersion()
    {
        if (isset($this->_packageInfo['_lastversion'])) {
            return $this->_packageInfo['_lastversion'];
        }
        return false;
    }

    /**
     * Determines whether this package.xml has post-install scripts or not
     * @return array|false
     */
    function listPostinstallScripts()
    {
        $filelist = $this->getFilelist();
        $contents = $this->getContents();
        $contents = $contents['dir']['file'];
        if (!is_array($contents) || !isset($contents[0])) {
            $contents = array($contents);
        }
        $taskfiles = array();
        foreach ($contents as $file) {
            $atts = $file['attribs'];
            unset($file['attribs']);
            if (count($file)) {
                $taskfiles[$atts['name']] = $file;
            }
        }
        $common = new PEAR_Common;
        $common->debug = $this->_config->get('verbose');
        $this->_scripts = array();
        $ret = array();
        foreach ($taskfiles as $name => $tasks) {
            if (!isset($filelist[$name])) {
                // ignored files will not be in the filelist
                continue;
            }
            $atts = $filelist[$name];
            foreach ($tasks as $tag => $raw) {
                $task = $this->getTask($tag);
                $task = &new $task($this->_config, $common, PEAR_TASK_INSTALL);
                if ($task->isScript()) {
                    $ret[] = $filelist[$name]['installed_as'];
                }
            }
        }
        if (count($ret)) {
            return $ret;
        }
        return false;
    }

    /**
     * Initialize post-install scripts for running
     *
     * This method can be used to detect post-install scripts, as the return value
     * indicates whether any exist
     * @return bool
     */
    function initPostinstallScripts()
    {
        $filelist = $this->getFilelist();
        $contents = $this->getContents();
        $contents = $contents['dir']['file'];
        if (!is_array($contents) || !isset($contents[0])) {
            $contents = array($contents);
        }
        $taskfiles = array();
        foreach ($contents as $file) {
            $atts = $file['attribs'];
            unset($file['attribs']);
            if (count($file)) {
                $taskfiles[$atts['name']] = $file;
            }
        }
        $common = new PEAR_Common;
        $common->debug = $this->_config->get('verbose');
        $this->_scripts = array();
        foreach ($taskfiles as $name => $tasks) {
            $atts = $filelist[$name];
            foreach ($tasks as $tag => $raw) {
                $taskname = $this->getTask($tag);
                $task = &new $taskname($this->_config, $common, PEAR_TASK_INSTALL);
                if (!$task->isScript()) {
                    continue; // scripts are only handled after installation
                }
                $lastversion = isset($this->_packageInfo['_lastversion']) ?
                    $this->_packageInfo['_lastversion'] : null;
                $task->init($raw, $atts, $lastversion);
                $res = $task->startSession($this, $atts['installed_as']);
                if (!$res) {
                    continue; // skip this file
                }
                if (PEAR::isError($res)) {
                    return $res;
                }
                $assign = &$task;
                $this->_scripts[] = &$assign;
            }
        }
        if (count($this->_scripts)) {
            return true;
        }
        return false;
    }

    function runPostinstallScripts()
    {
        if ($this->initPostinstallScripts()) {
            $ui = &PEAR_Frontend::singleton();
            if ($ui) {
                $ui->runPostinstallScripts($this->_scripts, $this);
            }
        }
    }


    /**
     * Convert a recursive set of <dir> and <file> tags into a single <dir> tag with
     * <file> tags.
     */
    function flattenFilelist()
    {
        if (isset($this->_packageInfo['bundle'])) {
            return;
        }
        $filelist = array();
        if (isset($this->_packageInfo['contents']['dir']['dir'])) {
            $this->_getFlattenedFilelist($filelist, $this->_packageInfo['contents']['dir']);
            if (!isset($filelist[1])) {
                $filelist = $filelist[0];
            }
            $this->_packageInfo['contents']['dir']['file'] = $filelist;
            unset($this->_packageInfo['contents']['dir']['dir']);
        } else {
            // else already flattened but check for baseinstalldir propagation
            if (isset($this->_packageInfo['contents']['dir']['attribs']['baseinstalldir'])) {
                if (isset($this->_packageInfo['contents']['dir']['file'][0])) {
                    foreach ($this->_packageInfo['contents']['dir']['file'] as $i => $file) {
                        if (isset($file['attribs']['baseinstalldir'])) {
                            continue;
                        }
                        $this->_packageInfo['contents']['dir']['file'][$i]['attribs']['baseinstalldir']
                            = $this->_packageInfo['contents']['dir']['attribs']['baseinstalldir'];
                    }
                } else {
                    if (!isset($this->_packageInfo['contents']['dir']['file']['attribs']['baseinstalldir'])) {
                       $this->_packageInfo['contents']['dir']['file']['attribs']['baseinstalldir']
                            = $this->_packageInfo['contents']['dir']['attribs']['baseinstalldir'];
                    }
                }
            }
        }
    }

    /**
     * @param array the final flattened file list
     * @param array the current directory being processed
     * @param string|false any recursively inherited baeinstalldir attribute
     * @param string private recursion variable
     * @return array
     * @access protected
     */
    function _getFlattenedFilelist(&$files, $dir, $baseinstall = false, $path = '')
    {
        if (isset($dir['attribs']) && isset($dir['attribs']['baseinstalldir'])) {
            $baseinstall = $dir['attribs']['baseinstalldir'];
        }
        if (isset($dir['dir'])) {
            if (!isset($dir['dir'][0])) {
                $dir['dir'] = array($dir['dir']);
            }
            foreach ($dir['dir'] as $subdir) {
                if (!isset($subdir['attribs']) || !isset($subdir['attribs']['name'])) {
                    $name = '*unknown*';
                } else {
                    $name = $subdir['attribs']['name'];
                }
                $newpath = empty($path) ? $name :
                    $path . '/' . $name;
                $this->_getFlattenedFilelist($files, $subdir,
                    $baseinstall, $newpath);
            }
        }
        if (isset($dir['file'])) {
            if (!isset($dir['file'][0])) {
                $dir['file'] = array($dir['file']);
            }
            foreach ($dir['file'] as $file) {
                $attrs = $file['attribs'];
                $name = $attrs['name'];
                if ($baseinstall && !isset($attrs['baseinstalldir'])) {
                    $attrs['baseinstalldir'] = $baseinstall;
                }
                $attrs['name'] = empty($path) ? $name : $path . '/' . $name;
                $attrs['name'] = preg_replace(array('!\\\\+!', '!/+!'), array('/', '/'),
                    $attrs['name']);
                $file['attribs'] = $attrs;
                $files[] = $file;
            }
        }
    }

    function setConfig(&$config)
    {
        $this->_config = &$config;
        $this->_registry = &$config->getRegistry();
    }

    function setLogger(&$logger)
    {
        if (!is_object($logger) || !method_exists($logger, 'log')) {
            return PEAR::raiseError('Logger must be compatible with PEAR_Common::log');
        }
        $this->_logger = &$logger;
    }

    /**
     * WARNING - do not use this function directly unless you know what you're doing
     */
    function setDeps($deps)
    {
        $this->_packageInfo['dependencies'] = $deps;
    }

    function setPackagefile($file, $archive = false)
    {
        $this->_packageFile = $file;
        $this->_archiveFile = $archive ? $archive : $file;
    }

    /**
     * Wrapper to {@link PEAR_ErrorStack::getErrors()}
     * @param boolean determines whether to purge the error stack after retrieving
     * @return array
     */
    function getValidationWarnings($purge = true)
    {
        return $this->_stack->getErrors($purge);
    }

    function getPackageFile()
    {
        return $this->_packageFile;
    }

    function getArchiveFile()
    {
        return $this->_archiveFile;
    }


    /**
     * Directly set the array that defines this packagefile
     *
     * WARNING: no validation.  This should only be performed by internal methods
     * inside PEAR or by inputting an array saved from an existing PEAR_PackageFile_v2
     * @param array
     */
    function fromArray($pinfo)
    {
        unset($pinfo['old']);
        unset($pinfo['xsdversion']);
        $this->_incomplete = false;
        $this->_packageInfo = $pinfo;
    }

    function isIncomplete()
    {
        return $this->_incomplete;
    }

    /**
     * @return array
     */
    function toArray($forreg = false)
    {
        if (!$this->validate(PEAR_VALIDATE_NORMAL)) {
            return false;
        }
        return $this->getArray($forreg);
    }

    function getArray($forReg = false)
    {
        if ($forReg) {
            $arr = $this->_packageInfo;
            $arr['old'] = array();
            $arr['old']['version'] = $this->getVersion();
            $arr['old']['release_date'] = $this->getDate();
            $arr['old']['release_state'] = $this->getState();
            $arr['old']['release_license'] = $this->getLicense();
            $arr['old']['release_notes'] = $this->getNotes();
            $arr['old']['release_deps'] = $this->getDeps();
            $arr['old']['maintainers'] = $this->getMaintainers();
            $arr['xsdversion'] = '2.0';
            return $arr;
        } else {
            $info = $this->_packageInfo;
            unset($info['dirtree']);
            if (isset($info['_lastversion'])) {
                unset($info['_lastversion']);
            }
            if (isset($info['#binarypackage'])) {
                unset($info['#binarypackage']);
            }
            return $info;
        }
    }

    function packageInfo($field)
    {
        $arr = $this->getArray(true);
        if ($field == 'state') {
            return $arr['stability']['release'];
        }
        if ($field == 'api-version') {
            return $arr['version']['api'];
        }
        if ($field == 'api-state') {
            return $arr['stability']['api'];
        }
        if (isset($arr['old'][$field])) {
            if (!is_string($arr['old'][$field])) {
                return null;
            }
            return $arr['old'][$field];
        }
        if (isset($arr[$field])) {
            if (!is_string($arr[$field])) {
                return null;
            }
            return $arr[$field];
        }
        return null;
    }

    function getName()
    {
        return $this->getPackage();
    }

    function getPackage()
    {
        if (isset($this->_packageInfo['name'])) {
            return $this->_packageInfo['name'];
        }
        return false;
    }

    function getChannel()
    {
        if (isset($this->_packageInfo['uri'])) {
            return '__uri';
        }
        if (isset($this->_packageInfo['channel'])) {
            return strtolower($this->_packageInfo['channel']);
        }
        return false;
    }

    function getUri()
    {
        if (isset($this->_packageInfo['uri'])) {
            return $this->_packageInfo['uri'];
        }
        return false;
    }

    function getExtends()
    {
        if (isset($this->_packageInfo['extends'])) {
            return $this->_packageInfo['extends'];
        }
        return false;
    }

    function getSummary()
    {
        if (isset($this->_packageInfo['summary'])) {
            return $this->_packageInfo['summary'];
        }
        return false;
    }

    function getDescription()
    {
        if (isset($this->_packageInfo['description'])) {
            return $this->_packageInfo['description'];
        }
        return false;
    }

    function getMaintainers($raw = false)
    {
        if (!isset($this->_packageInfo['lead'])) {
            return false;
        }
        if ($raw) {
            $ret = array('lead' => $this->_packageInfo['lead']);
            (isset($this->_packageInfo['developer'])) ?
                $ret['developer'] = $this->_packageInfo['developer'] :null;
            (isset($this->_packageInfo['contributor'])) ?
                $ret['contributor'] = $this->_packageInfo['contributor'] :null;
            (isset($this->_packageInfo['helper'])) ?
                $ret['helper'] = $this->_packageInfo['helper'] :null;
            return $ret;
        } else {
            $ret = array();
            $leads = isset($this->_packageInfo['lead'][0]) ? $this->_packageInfo['lead'] :
                array($this->_packageInfo['lead']);
            foreach ($leads as $lead) {
                $s = $lead;
                $s['handle'] = $s['user'];
                unset($s['user']);
                $s['role'] = 'lead';
                $ret[] = $s;
            }
            if (isset($this->_packageInfo['developer'])) {
                $leads = isset($this->_packageInfo['developer'][0]) ?
                    $this->_packageInfo['developer'] :
                    array($this->_packageInfo['developer']);
                foreach ($leads as $maintainer) {
                    $s = $maintainer;
                    $s['handle'] = $s['user'];
                    unset($s['user']);
                    $s['role'] = 'developer';
                    $ret[] = $s;
                }
            }
            if (isset($this->_packageInfo['contributor'])) {
                $leads = isset($this->_packageInfo['contributor'][0]) ?
                    $this->_packageInfo['contributor'] :
                    array($this->_packageInfo['contributor']);
                foreach ($leads as $maintainer) {
                    $s = $maintainer;
                    $s['handle'] = $s['user'];
                    unset($s['user']);
                    $s['role'] = 'contributor';
                    $ret[] = $s;
                }
            }
            if (isset($this->_packageInfo['helper'])) {
                $leads = isset($this->_packageInfo['helper'][0]) ?
                    $this->_packageInfo['helper'] :
                    array($this->_packageInfo['helper']);
                foreach ($leads as $maintainer) {
                    $s = $maintainer;
                    $s['handle'] = $s['user'];
                    unset($s['user']);
                    $s['role'] = 'helper';
                    $ret[] = $s;
                }
            }
            return $ret;
        }
        return false;
    }

    function getLeads()
    {
        if (isset($this->_packageInfo['lead'])) {
            return $this->_packageInfo['lead'];
        }
        return false;
    }

    function getDevelopers()
    {
        if (isset($this->_packageInfo['developer'])) {
            return $this->_packageInfo['developer'];
        }
        return false;
    }

    function getContributors()
    {
        if (isset($this->_packageInfo['contributor'])) {
            return $this->_packageInfo['contributor'];
        }
        return false;
    }

    function getHelpers()
    {
        if (isset($this->_packageInfo['helper'])) {
            return $this->_packageInfo['helper'];
        }
        return false;
    }

    function setDate($date)
    {
        if (!isset($this->_packageInfo['date'])) {
            // ensure that the extends tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('time', 'version',
                    'stability', 'license', 'notes', 'contents', 'compatible',
                    'dependencies', 'providesextension', 'srcpackage', 'srcuri',
                    'phprelease', 'extsrcrelease',
                    'extbinrelease', 'bundle', 'changelog'), array(), 'date');
        }
        $this->_packageInfo['date'] = $date;
        $this->_isValid = 0;
    }

    function setTime($time)
    {
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['time'])) {
            // ensure that the time tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                    array('version',
                    'stability', 'license', 'notes', 'contents', 'compatible',
                    'dependencies', 'providesextension', 'srcpackage', 'srcuri',
                    'phprelease', 'extsrcrelease',
                    'extbinrelease', 'bundle', 'changelog'), $time, 'time');
        }
        $this->_packageInfo['time'] = $time;
    }

    function getDate()
    {
        if (isset($this->_packageInfo['date'])) {
            return $this->_packageInfo['date'];
        }
        return false;
    }

    function getTime()
    {
        if (isset($this->_packageInfo['time'])) {
            return $this->_packageInfo['time'];
        }
        return false;
    }

    /**
     * @param package|api version category to return
     */
    function getVersion($key = 'release')
    {
        if (isset($this->_packageInfo['version'][$key])) {
            return $this->_packageInfo['version'][$key];
        }
        return false;
    }

    function getStability()
    {
        if (isset($this->_packageInfo['stability'])) {
            return $this->_packageInfo['stability'];
        }
        return false;
    }

    function getState($key = 'release')
    {
        if (isset($this->_packageInfo['stability'][$key])) {
            return $this->_packageInfo['stability'][$key];
        }
        return false;
    }

    function getLicense($raw = false)
    {
        if (isset($this->_packageInfo['license'])) {
            if ($raw) {
                return $this->_packageInfo['license'];
            }
            if (is_array($this->_packageInfo['license'])) {
                return $this->_packageInfo['license']['_content'];
            } else {
                return $this->_packageInfo['license'];
            }
        }
        return false;
    }

    function getLicenseLocation()
    {
        if (!isset($this->_packageInfo['license']) || !is_array($this->_packageInfo['license'])) {
            return false;
        }
        return $this->_packageInfo['license']['attribs'];
    }

    function getNotes()
    {
        if (isset($this->_packageInfo['notes'])) {
            return $this->_packageInfo['notes'];
        }
        return false;
    }

    /**
     * Return the <usesrole> tag contents, if any
     * @return array|false
     */
    function getUsesrole()
    {
        if (isset($this->_packageInfo['usesrole'])) {
            return $this->_packageInfo['usesrole'];
        }
        return false;
    }

    /**
     * Return the <usestask> tag contents, if any
     * @return array|false
     */
    function getUsestask()
    {
        if (isset($this->_packageInfo['usestask'])) {
            return $this->_packageInfo['usestask'];
        }
        return false;
    }

    /**
     * This should only be used to retrieve filenames and install attributes
     */
    function getFilelist($preserve = false)
    {
        if (isset($this->_packageInfo['filelist']) && !$preserve) {
            return $this->_packageInfo['filelist'];
        }
        $this->flattenFilelist();
        if ($contents = $this->getContents()) {
            $ret = array();
            if (!isset($contents['dir']['file'][0])) {
                $contents['dir']['file'] = array($contents['dir']['file']);
            }
            foreach ($contents['dir']['file'] as $file) {
                $name = $file['attribs']['name'];
                if (!$preserve) {
                    $file = $file['attribs'];
                }
                $ret[$name] = $file;
            }
            if (!$preserve) {
                $this->_packageInfo['filelist'] = $ret;
            }
            return $ret;
        }
        return false;
    }

    /**
     * Return configure options array, if any
     *
     * @return array|false
     */
    function getConfigureOptions()
    {
        if ($this->getPackageType() != 'extsrc') {
            return false;
        }
        $releases = $this->getReleases();
        if (isset($releases[0])) {
            $releases = $release[0];
        }
        if (isset($releases['configureoption'])) {
            if (!isset($releases['configureoption'][0])) {
                $releases['configureoption'] = array($releases['configureoption']);
            }
            for ($i = 0; $i < count($releases['configureoption']); $i++) {
                $releases['configureoption'][$i] = $releases['configureoption'][$i]['attribs'];
            }
            return $releases['configureoption'];
        }
        return false;
    }

    /**
     * This is only used at install-time, after all serialization
     * is over.
     */
    function resetFilelist()
    {
        $this->_packageInfo['filelist'] = array();
    }

    /**
     * Retrieve a list of files that should be installed on this computer
     * @return array
     */
    function getInstallationFilelist($forfilecheck = false)
    {
        $contents = $this->getFilelist(true);
        if (isset($contents['dir']['attribs']['baseinstalldir'])) {
            $base = $contents['dir']['attribs']['baseinstalldir'];
        }
        if (isset($this->_packageInfo['bundle'])) {
            return PEAR::raiseError(
                'Exception: bundles should be handled in download code only');
        }
        $release = $this->getReleases();
        if ($release) {
            if (!isset($release[0])) {
                if (!isset($release['installconditions']) && !isset($release['filelist'])) {
                    if ($forfilecheck) {
                        return $this->getFilelist();
                    }
                    return $contents;
                }
                $release = array($release);
            }
            $depchecker = &$this->getPEARDependency2($this->_config, array(),
                array('channel' => $this->getChannel(), 'package' => $this->getPackage()),
                PEAR_VALIDATE_INSTALLING);
            foreach ($release as $instance) {
                if (isset($instance['installconditions'])) {
                    $installconditions = $instance['installconditions'];
                    if (is_array($installconditions)) {
                        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
                        foreach ($installconditions as $type => $conditions) {
                            if (!isset($conditions[0])) {
                                $conditions = array($conditions);
                            }
                            foreach ($conditions as $condition) {
                                $ret = $depchecker->{"validate{$type}Dependency"}($condition);
                                if (PEAR::isError($ret)) {
                                    PEAR::popErrorHandling();
                                    continue 3; // skip this release
                                }
                            }
                        }
                        PEAR::popErrorHandling();
                    }
                }
                // this is the release to use
                if (isset($instance['filelist'])) {
                    // ignore files
                    if (isset($instance['filelist']['ignore'])) {
                        $ignore = isset($instance['filelist']['ignore'][0]) ?
                            $instance['filelist']['ignore'] :
                            array($instance['filelist']['ignore']);
                        foreach ($ignore as $ig) {
                            unset ($contents[$ig['attribs']['name']]);
                        }
                    }
                    // install files as this name
                    if (isset($instance['filelist']['install'])) {
                        $installas = isset($instance['filelist']['install'][0]) ?
                            $instance['filelist']['install'] :
                            array($instance['filelist']['install']);
                        foreach ($installas as $as) {
                            $contents[$as['attribs']['name']]['attribs']['install-as'] =
                                $as['attribs']['as'];
                        }
                    }
                }
                if ($forfilecheck) {
                    foreach ($contents as $file => $attrs) {
                        $contents[$file] = $attrs['attribs'];
                    }
                }
                return $contents;
            }
        } else { // simple release - no installconditions or install-as
            if ($forfilecheck) {
                return $this->getFilelist();
            }
            return $contents;
        }
        // no releases matched
        return PEAR::raiseError('No releases in package.xml matched the existing operating ' .
            'system, extensions installed, or architecture, cannot install');
    }

    /**
     * This is only used at install-time, after all serialization
     * is over.
     * @param string file name
     * @param string installed path
     */
    function setInstalledAs($file, $path)
    {
        if ($path) {
            return $this->_packageInfo['filelist'][$file]['installed_as'] = $path;
        }
        unset($this->_packageInfo['filelist'][$file]['installed_as']);
    }

    function getInstalledLocation($file)
    {
        if (isset($this->_packageInfo['filelist'][$file]['installed_as'])) {
            return $this->_packageInfo['filelist'][$file]['installed_as'];
        }
        return false;
    }

    /**
     * This is only used at install-time, after all serialization
     * is over.
     */
    function installedFile($file, $atts)
    {
        if (isset($this->_packageInfo['filelist'][$file])) {
            $this->_packageInfo['filelist'][$file] =
                array_merge($this->_packageInfo['filelist'][$file], $atts['attribs']);
        } else {
            $this->_packageInfo['filelist'][$file] = $atts['attribs'];
        }
    }

    /**
     * Retrieve the contents tag
     */
    function getContents()
    {
        if (isset($this->_packageInfo['contents'])) {
            return $this->_packageInfo['contents'];
        }
        return false;
    }

    /**
     * @param string full path to file
     * @param string attribute name
     * @param string attribute value
     * @param int risky but fast - use this to choose a file based on its position in the list
     *            of files.  Index is zero-based like PHP arrays.
     * @return bool success of operation
     */
    function setFileAttribute($filename, $attr, $value, $index = false)
    {
        $this->_isValid = 0;
        if (in_array($attr, array('role', 'name', 'baseinstalldir'))) {
            $this->_filesValid = false;
        }
        if ($index !== false &&
              isset($this->_packageInfo['contents']['dir']['file'][$index]['attribs'])) {
            $this->_packageInfo['contents']['dir']['file'][$index]['attribs'][$attr] = $value;
            return true;
        }
        if (!isset($this->_packageInfo['contents']['dir']['file'])) {
            return false;
        }
        $files = $this->_packageInfo['contents']['dir']['file'];
        if (!isset($files[0])) {
            $files = array($files);
            $ind = false;
        } else {
            $ind = true;
        }
        foreach ($files as $i => $file) {
            if (isset($file['attribs'])) {
                if ($file['attribs']['name'] == $filename) {
                    if ($ind) {
                        $this->_packageInfo['contents']['dir']['file'][$i]['attribs'][$attr] = $value;
                    } else {
                        $this->_packageInfo['contents']['dir']['file']['attribs'][$attr] = $value;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    function setDirtree($path)
    {
        $this->_packageInfo['dirtree'][$path] = true;
    }

    function getDirtree()
    {
        if (isset($this->_packageInfo['dirtree']) && count($this->_packageInfo['dirtree'])) {
            return $this->_packageInfo['dirtree'];
        }
        return false;
    }

    function resetDirtree()
    {
        unset($this->_packageInfo['dirtree']);
    }

    /**
     * Determines whether this package claims it is compatible with the version of
     * the package that has a recommended version dependency
     * @param PEAR_PackageFile_v2|PEAR_PackageFile_v1|PEAR_Downloader_Package
     * @return boolean
     */
    function isCompatible($pf)
    {
        if (!isset($this->_packageInfo['compatible'])) {
            return false;
        }
        if (!isset($this->_packageInfo['channel'])) {
            return false;
        }
        $me = $pf->getVersion();
        $compatible = $this->_packageInfo['compatible'];
        if (!isset($compatible[0])) {
            $compatible = array($compatible);
        }
        $found = false;
        foreach ($compatible as $info) {
            if (strtolower($info['name']) == strtolower($pf->getPackage())) {
                if (strtolower($info['channel']) == strtolower($pf->getChannel())) {
                    $found = true;
                    break;
                }
            }
        }
        if (!$found) {
            return false;
        }
        if (isset($info['exclude'])) {
            if (!isset($info['exclude'][0])) {
                $info['exclude'] = array($info['exclude']);
            }
            foreach ($info['exclude'] as $exclude) {
                if (version_compare($me, $exclude, '==')) {
                    return false;
                }
            }
        }
        if (version_compare($me, $info['min'], '>=') && version_compare($me, $info['max'], '<=')) {
            return true;
        }
        return false;
    }

    /**
     * @return array|false
     */
    function getCompatible()
    {
        if (isset($this->_packageInfo['compatible'])) {
            return $this->_packageInfo['compatible'];
        }
        return false;
    }

    function getDependencies()
    {
        if (isset($this->_packageInfo['dependencies'])) {
            return $this->_packageInfo['dependencies'];
        }
        return false;
    }

    function isSubpackageOf($p)
    {
        return $p->isSubpackage($this);
    }

    /**
     * Determines whether the passed in package is a subpackage of this package.
     *
     * No version checking is done, only name verification.
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @return bool
     */
    function isSubpackage($p)
    {
        $sub = array();
        if (isset($this->_packageInfo['dependencies']['required']['subpackage'])) {
            $sub = $this->_packageInfo['dependencies']['required']['subpackage'];
            if (!isset($sub[0])) {
                $sub = array($sub);
            }
        }
        if (isset($this->_packageInfo['dependencies']['optional']['subpackage'])) {
            $sub1 = $this->_packageInfo['dependencies']['optional']['subpackage'];
            if (!isset($sub1[0])) {
                $sub1 = array($sub1);
            }
            $sub = array_merge($sub, $sub1);
        }
        if (isset($this->_packageInfo['dependencies']['group'])) {
            $group = $this->_packageInfo['dependencies']['group'];
            if (!isset($group[0])) {
                $group = array($group);
            }
            foreach ($group as $deps) {
                if (isset($deps['subpackage'])) {
                    $sub2 = $deps['subpackage'];
                    if (!isset($sub2[0])) {
                        $sub2 = array($sub2);
                    }
                    $sub = array_merge($sub, $sub2);
                }
            }
        }
        foreach ($sub as $dep) {
            if (strtolower($dep['name']) == strtolower($p->getPackage())) {
                if (isset($dep['channel'])) {
                    if (strtolower($dep['channel']) == strtolower($p->getChannel())) {
                        return true;
                    }
                } else {
                    if ($dep['uri'] == $p->getURI()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    function dependsOn($package, $channel)
    {
        if (!($deps = $this->getDependencies())) {
            return false;
        }
        foreach (array('package', 'subpackage') as $type) {
            foreach (array('required', 'optional') as $needed) {
                if (isset($deps[$needed][$type])) {
                    if (!isset($deps[$needed][$type][0])) {
                        $deps[$needed][$type] = array($deps[$needed][$type]);
                    }
                    foreach ($deps[$needed][$type] as $dep) {
                        $depchannel = isset($dep['channel']) ? $dep['channel'] : '__uri';
                        if (strtolower($dep['name']) == strtolower($package) &&
                              $depchannel == $channel) {
                            return true;
                        }  
                    }
                }
            }
            if (isset($deps['group'])) {
                if (!isset($deps['group'][0])) {
                    $dep['group'] = array($deps['group']);
                }
                foreach ($deps['group'] as $group) {
                    if (isset($group[$type])) {
                        if (!is_array($group[$type])) {
                            $group[$type] = array($group[$type]);
                        }
                        foreach ($group[$type] as $dep) {
                            $depchannel = isset($dep['channel']) ? $dep['channel'] : '__uri';
                            if (strtolower($dep['name']) == strtolower($package) &&
                                  $depchannel == $channel) {
                                return true;
                            }  
                        }
                    }
                }
            }
        }
        return false;
    }

    /**
     * Get the contents of a dependency group
     * @param string
     * @return array|false
     */
    function getDependencyGroup($name)
    {
        $name = strtolower($name);
        if (!isset($this->_packageInfo['dependencies']['group'])) {
            return false;
        }
        $groups = $this->_packageInfo['dependencies']['group'];
        if (!isset($groups[0])) {
            $groups = array($groups);
        }
        foreach ($groups as $group) {
            if (strtolower($group['attribs']['name']) == $name) {
                return $group;
            }
        }
        return false;
    }

    /**
     * Retrieve a partial package.xml 1.0 representation of dependencies
     *
     * a very limited representation of dependencies is returned by this method.
     * The <exclude> tag for excluding certain versions of a dependency is
     * completely ignored.  In addition, dependency groups are ignored, with the
     * assumption that all dependencies in dependency groups are also listed in
     * the optional group that work with all dependency groups
     * @param boolean return package.xml 2.0 <dependencies> tag
     * @return array|false
     */
    function getDeps($raw = false, $nopearinstaller = false)
    {
        if (isset($this->_packageInfo['dependencies'])) {
            if ($raw) {
                return $this->_packageInfo['dependencies'];
            }
            $ret = array();
            $map = array(
                'php' => 'php',
                'package' => 'pkg',
                'subpackage' => 'pkg',
                'extension' => 'ext',
                'os' => 'os',
                'pearinstaller' => 'pkg',
                );
            foreach (array('required', 'optional') as $type) {
                $optional = ($type == 'optional') ? 'yes' : 'no';
                if (!isset($this->_packageInfo['dependencies'][$type])) {
                    continue;
                }
                foreach ($this->_packageInfo['dependencies'][$type] as $dtype => $deps) {
                    if ($dtype == 'pearinstaller' && $nopearinstaller) {
                        continue;
                    }
                    if (!isset($deps[0])) {
                        $deps = array($deps);
                    }
                    foreach ($deps as $dep) {
                        if (!isset($map[$dtype])) {
                            // no support for arch type
                            continue;
                        }
                        if ($dtype == 'pearinstaller') {
                            $dep['name'] = 'PEAR';
                            $dep['channel'] = 'pear.php.net';
                        }
                        $s = array('type' => $map[$dtype]);
                        if (isset($dep['channel'])) {
                            $s['channel'] = $dep['channel'];
                        }
                        if (isset($dep['uri'])) {
                            $s['uri'] = $dep['uri'];
                        }
                        if (isset($dep['name'])) {
                            $s['name'] = $dep['name'];
                        }
                        if (isset($dep['conflicts'])) {
                            $s['rel'] = 'not';
                        } else {
                            if (!isset($dep['min']) &&
                                  !isset($dep['max'])) {
                                $s['rel'] = 'has';
                                $s['optional'] = $optional;
                            } elseif (isset($dep['min']) &&
                                  isset($dep['max'])) {
                                $s['rel'] = 'ge';
                                $s1 = $s;
                                $s1['rel'] = 'le';
                                $s['version'] = $dep['min'];
                                $s1['version'] = $dep['max'];
                                if (isset($dep['channel'])) {
                                    $s1['channel'] = $dep['channel'];
                                }
                                if ($dtype != 'php') {
                                    $s['name'] = $dep['name'];
                                    $s1['name'] = $dep['name'];
                                }
                                $s['optional'] = $optional;
                                $s1['optional'] = $optional;
                                $ret[] = $s1;
                            } elseif (isset($dep['min'])) {
                                if (isset($dep['exclude']) &&
                                      $dep['exclude'] == $dep['min']) {
                                    $s['rel'] = 'gt';
                                } else {
                                    $s['rel'] = 'ge';
                                }
                                $s['version'] = $dep['min'];
                                $s['optional'] = $optional;
                                if ($dtype != 'php') {
                                    $s['name'] = $dep['name'];
                                }
                            } elseif (isset($dep['max'])) {
                                if (isset($dep['exclude']) &&
                                      $dep['exclude'] == $dep['max']) {
                                    $s['rel'] = 'lt';
                                } else {
                                    $s['rel'] = 'le';
                                }
                                $s['version'] = $dep['max'];
                                $s['optional'] = $optional;
                                if ($dtype != 'php') {
                                    $s['name'] = $dep['name'];
                                }
                            }
                        }
                        $ret[] = $s;
                    }
                }
            }
            if (count($ret)) {
                return $ret;
            }
        }
        return false;
    }

    /**
     * @return php|extsrc|extbin|bundle|false
     */
    function getPackageType()
    {
        if (isset($this->_packageInfo['phprelease'])) {
            return 'php';
        }
        if (isset($this->_packageInfo['extsrcrelease'])) {
            return 'extsrc';
        }
        if (isset($this->_packageInfo['extbinrelease'])) {
            return 'extbin';
        }
        if (isset($this->_packageInfo['bundle'])) {
            return 'bundle';
        }
        return false;
    }

    /**
     * @return array|false
     */
    function getReleases()
    {
        $type = $this->getPackageType();
        if ($type != 'bundle') {
            $type .= 'release';
        }
        if ($this->getPackageType() && isset($this->_packageInfo[$type])) {
            return $this->_packageInfo[$type];
        }
        return false;
    }

    /**
     * @return array
     */
    function getChangelog()
    {
        if (isset($this->_packageInfo['changelog'])) {
            return $this->_packageInfo['changelog'];
        }
        return false;
    }

    function hasDeps()
    {
        return isset($this->_packageInfo['dependencies']);
    }

    function getPackagexmlVersion()
    {
        return '2.0';
    }

    /**
     * @return array|false
     */
    function getSourcePackage()
    {
        if (isset($this->_packageInfo['extbinrelease'])) {
            return array('channel' => $this->_packageInfo['srcchannel'],
                         'package' => $this->_packageInfo['srcpackage']);
        }
        return false;
    }

    function getBundledPackages()
    {
        if (isset($this->_packageInfo['bundle'])) {
            return $this->_packageInfo['contents']['bundledpackage'];
        }
        return false;
    }

    function getLastModified()
    {
        if (isset($this->_packageInfo['_lastmodified'])) {
            return $this->_packageInfo['_lastmodified'];
        }
        return false;
    }

    /**
     * Get the contents of a file listed within the package.xml
     * @param string
     * @return string
     */
    function getFileContents($file)
    {
        if ($this->_archiveFile == $this->_packageFile) { // unpacked
            $dir = dirname($this->_packageFile);
            $file = $dir . DIRECTORY_SEPARATOR . $file;
            $file = str_replace(array('/', '\\'),
                array(DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR), $file);
            if (file_exists($file) && is_readable($file)) {
                return implode('', file($file));
            }
        } else { // tgz
            $tar = &new Archive_Tar($this->_archiveFile);
            $tar->pushErrorHandling(PEAR_ERROR_RETURN);
            if ($file != 'package.xml' && $file != 'package2.xml') {
                $file = $this->getPackage() . '-' . $this->getVersion() . '/' . $file;
            }
            $file = $tar->extractInString($file);
            $tar->popErrorHandling();
            if (PEAR::isError($file)) {
                return PEAR::raiseError("Cannot locate file '$file' in archive");
            }
            return $file;
        }
    }

    function &getRW()
    {
        if (!class_exists('PEAR_PackageFile_v2_rw')) {
            require_once 'PEAR/PackageFile/v2/rw.php';
        }
        $a = new PEAR_PackageFile_v2_rw;
        foreach (get_object_vars($this) as $name => $unused) {
            if (!isset($this->$name)) {
                continue;
            }
            if ($name == '_config' || $name == '_logger'|| $name == '_registry' ||
                  $name == '_stack') {
                $a->$name = &$this->$name;
            } else {
                $a->$name = $this->$name;
            }
        }
        return $a;
    }

    function &getDefaultGenerator()
    {
        if (!class_exists('PEAR_PackageFile_Generator_v2')) {
            require_once 'PEAR/PackageFile/Generator/v2.php';
        }
        $a = &new PEAR_PackageFile_Generator_v2($this);
        return $a;
    }

    function analyzeSourceCode($file, $string = false)
    {
        if (!isset($this->_v2Validator) ||
              !is_a($this->_v2Validator, 'PEAR_PackageFile_v2_Validator')) {
            if (!class_exists('PEAR_PackageFile_v2_Validator')) {
                require_once 'PEAR/PackageFile/v2/Validator.php';
            }
            $this->_v2Validator = new PEAR_PackageFile_v2_Validator;
        }
        return $this->_v2Validator->analyzeSourceCode($file, $string);
    }

    function validate($state = PEAR_VALIDATE_NORMAL)
    {
        if (!isset($this->_packageInfo) || !is_array($this->_packageInfo)) {
            return false;
        }
        if (!isset($this->_v2Validator) ||
              !is_a($this->_v2Validator, 'PEAR_PackageFile_v2_Validator')) {
            if (!class_exists('PEAR_PackageFile_v2_Validator')) {
                require_once 'PEAR/PackageFile/v2/Validator.php';
            }
            $this->_v2Validator = new PEAR_PackageFile_v2_Validator;
        }
        if (isset($this->_packageInfo['xsdversion'])) {
            unset($this->_packageInfo['xsdversion']);
        }
        return $this->_v2Validator->validate($this, $state);
    }

    function getTasksNs()
    {
        if (!isset($this->_tasksNs)) {
            if (isset($this->_packageInfo['attribs'])) {
                foreach ($this->_packageInfo['attribs'] as $name => $value) {
                    if ($value == 'http://pear.php.net/dtd/tasks-1.0') {
                        $this->_tasksNs = str_replace('xmlns:', '', $name);
                        break;
                    }
                }
            }
        }
        return $this->_tasksNs;
    }

    /**
     * Determine whether a task name is a valid task.  Custom tasks may be defined
     * using subdirectories by putting a "-" in the name, as in <tasks:mycustom-task>
     *
     * Note that this method will auto-load the task class file and test for the existence
     * of the name with "-" replaced by "_" as in PEAR/Task/mycustom/task.php makes class
     * PEAR_Task_mycustom_task
     * @param string
     * @return boolean
     */
    function getTask($task)
    {
        $this->getTasksNs();
        // transform all '-' to '/' and 'tasks:' to '' so tasks:replace becomes replace
        $task = str_replace(array($this->_tasksNs . ':', '-'), array('', ' '), $task);
        $task = str_replace(' ', '/', ucwords($task));
        $ps = (strtolower(substr(PHP_OS, 0, 3)) == 'win') ? ';' : ':';
        foreach (explode($ps, ini_get('include_path')) as $path) {
            if (file_exists($path . "/PEAR/Task/$task.php")) {
                include_once "PEAR/Task/$task.php";
                $task = str_replace('/', '_', $task);
                if (class_exists("PEAR_Task_$task")) {
                    return "PEAR_Task_$task";
                }
            }
        }
        return false;
    }

    /**
     * Key-friendly array_splice
     * @param tagname to splice a value in before
     * @param mixed the value to splice in
     * @param string the new tag name
     */
    function _ksplice($array, $key, $value, $newkey)
    {
        $offset = array_search($key, array_keys($array));
        $after = array_slice($array, $offset);
        $before = array_slice($array, 0, $offset);
        $before[$newkey] = $value;
        return array_merge($before, $after);
    }

    /**
     * @param array a list of possible keys, in the order they may occur
     * @param mixed contents of the new package.xml tag
     * @param string tag name
     * @access private
     */
    function _insertBefore($array, $keys, $contents, $newkey)
    {
        foreach ($keys as $key) {
            if (isset($array[$key])) {
                return $array = $this->_ksplice($array, $key, $contents, $newkey);
            }
        }
        $array[$newkey] = $contents;
        return $array;
    }

    /**
     * @param subsection of {@link $_packageInfo}
     * @param array|string tag contents
     * @param array format:
     * <pre>
     * array(
     *   tagname => array(list of tag names that follow this one),
     *   childtagname => array(list of child tag names that follow this one),
     * )
     * </pre>
     *
     * This allows construction of nested tags
     * @access private
     */
    function _mergeTag($manip, $contents, $order)
    {
        if (count($order)) {
            foreach ($order as $tag => $curorder) {
                if (!isset($manip[$tag])) {
                    // ensure that the tag is set up
                    $manip = $this->_insertBefore($manip, $curorder, array(), $tag);
                }
                if (count($order) > 1) {
                    $manip[$tag] = $this->_mergeTag($manip[$tag], $contents, array_slice($order, 1));
                    return $manip;
                }
            }
        } else {
            return $manip;
        }
        if (is_array($manip[$tag]) && !empty($manip[$tag]) && isset($manip[$tag][0])) {
            $manip[$tag][] = $contents;
        } else {
            if (!count($manip[$tag])) {
                $manip[$tag] = $contents;
            } else {
                $manip[$tag] = array($manip[$tag]);
                $manip[$tag][] = $contents;
            }
        }
        return $manip;
    }
}
?>
