<?php
/**
 * PEAR_Common, the base class for the PEAR Installer
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
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V. V. Cox <cox@idecnet.com>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1.0
 * @deprecated File deprecated since Release 1.4.0a1
 */

/**
 * Include error handling
 */
require_once 'PEAR.php';

// {{{ constants and globals

/**
 * PEAR_Common error when an invalid PHP file is passed to PEAR_Common::analyzeSourceCode()
 */
define('PEAR_COMMON_ERROR_INVALIDPHP', 1);
define('_PEAR_COMMON_PACKAGE_NAME_PREG', '[A-Za-z][a-zA-Z0-9_]+');
define('PEAR_COMMON_PACKAGE_NAME_PREG', '/^' . _PEAR_COMMON_PACKAGE_NAME_PREG . '$/');

// this should allow: 1, 1.0, 1.0RC1, 1.0dev, 1.0dev123234234234, 1.0a1, 1.0b1, 1.0pl1
define('_PEAR_COMMON_PACKAGE_VERSION_PREG', '\d+(?:\.\d+)*(?:[a-zA-Z]+\d*)?');
define('PEAR_COMMON_PACKAGE_VERSION_PREG', '/^' . _PEAR_COMMON_PACKAGE_VERSION_PREG . '$/i');

// XXX far from perfect :-)
define('_PEAR_COMMON_PACKAGE_DOWNLOAD_PREG', '(' . _PEAR_COMMON_PACKAGE_NAME_PREG .
    ')(-([.0-9a-zA-Z]+))?');
define('PEAR_COMMON_PACKAGE_DOWNLOAD_PREG', '/^' . _PEAR_COMMON_PACKAGE_DOWNLOAD_PREG .
    '$/');

define('_PEAR_CHANNELS_NAME_PREG', '[A-Za-z][a-zA-Z0-9\.]+');
define('PEAR_CHANNELS_NAME_PREG', '/^' . _PEAR_CHANNELS_NAME_PREG . '$/');

// this should allow any dns or IP address, plus a path - NO UNDERSCORES ALLOWED
define('_PEAR_CHANNELS_SERVER_PREG', '[a-zA-Z0-9\-]+(?:\.[a-zA-Z0-9\-]+)*(\/[a-zA-Z0-9\-]+)*');
define('PEAR_CHANNELS_SERVER_PREG', '/^' . _PEAR_CHANNELS_SERVER_PREG . '$/i');

define('_PEAR_CHANNELS_PACKAGE_PREG',  '(' ._PEAR_CHANNELS_SERVER_PREG . ')\/('
         . _PEAR_COMMON_PACKAGE_NAME_PREG . ')');
define('PEAR_CHANNELS_PACKAGE_PREG', '/^' . _PEAR_CHANNELS_PACKAGE_PREG . '$/i');

define('_PEAR_COMMON_CHANNEL_DOWNLOAD_PREG', '(' . _PEAR_CHANNELS_NAME_PREG . ')::('
    . _PEAR_COMMON_PACKAGE_NAME_PREG . ')(-([.0-9a-zA-Z]+))?');
define('PEAR_COMMON_CHANNEL_DOWNLOAD_PREG', '/^' . _PEAR_COMMON_CHANNEL_DOWNLOAD_PREG . '$/');

/**
 * List of temporary files and directories registered by
 * PEAR_Common::addTempFile().
 * @var array
 */
$GLOBALS['_PEAR_Common_tempfiles'] = array();

/**
 * Valid maintainer roles
 * @var array
 */
$GLOBALS['_PEAR_Common_maintainer_roles'] = array('lead','developer','contributor','helper');

/**
 * Valid release states
 * @var array
 */
$GLOBALS['_PEAR_Common_release_states'] = array('alpha','beta','stable','snapshot','devel');

/**
 * Valid dependency types
 * @var array
 */
$GLOBALS['_PEAR_Common_dependency_types'] = array('pkg','ext','php','prog','ldlib','rtlib','os','websrv','sapi');

/**
 * Valid dependency relations
 * @var array
 */
$GLOBALS['_PEAR_Common_dependency_relations'] = array('has','eq','lt','le','gt','ge','not', 'ne');

/**
 * Valid file roles
 * @var array
 */
$GLOBALS['_PEAR_Common_file_roles'] = array('php','ext','test','doc','data','src','script');

/**
 * Valid replacement types
 * @var array
 */
$GLOBALS['_PEAR_Common_replacement_types'] = array('php-const', 'pear-config', 'package-info');

/**
 * Valid "provide" types
 * @var array
 */
$GLOBALS['_PEAR_Common_provide_types'] = array('ext', 'prog', 'class', 'function', 'feature', 'api');

/**
 * Valid "provide" types
 * @var array
 */
$GLOBALS['_PEAR_Common_script_phases'] = array('pre-install', 'post-install', 'pre-uninstall', 'post-uninstall', 'pre-build', 'post-build', 'pre-configure', 'post-configure', 'pre-setup', 'post-setup');

// }}}

/**
 * Class providing common functionality for PEAR administration classes.
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V. V. Cox <cox@idecnet.com>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 * @deprecated This class will disappear, and its components will be spread
 *             into smaller classes, like the AT&T breakup, as of Release 1.4.0a1
 */
class PEAR_Common extends PEAR
{
    // {{{ properties

    /** stack of elements, gives some sort of XML context */
    var $element_stack = array();

    /** name of currently parsed XML element */
    var $current_element;

    /** array of attributes of the currently parsed XML element */
    var $current_attributes = array();

    /** assoc with information about a package */
    var $pkginfo = array();

    /**
     * User Interface object (PEAR_Frontend_* class).  If null,
     * the log() method uses print.
     * @var object
     */
    var $ui = null;

    /**
     * Configuration object (PEAR_Config).
     * @var object
     */
    var $config = null;

    var $current_path = null;

    /**
     * PEAR_SourceAnalyzer instance
     * @var object
     */
    var $source_analyzer = null;
    /**
     * Flag variable used to mark a valid package file
     * @var boolean
     * @access private
     */
    var $_validPackageFile;

    // }}}

    // {{{ constructor

    /**
     * PEAR_Common constructor
     *
     * @access public
     */
    function PEAR_Common()
    {
        parent::PEAR();
        $this->config = &PEAR_Config::singleton();
        $this->debug = $this->config->get('verbose');
    }

    // }}}
    // {{{ destructor

    /**
     * PEAR_Common destructor
     *
     * @access private
     */
    function _PEAR_Common()
    {
        // doesn't work due to bug #14744
        //$tempfiles = $this->_tempfiles;
        $tempfiles =& $GLOBALS['_PEAR_Common_tempfiles'];
        while ($file = array_shift($tempfiles)) {
            if (@is_dir($file)) {
                if (!class_exists('System')) {
                    require_once 'System.php';
                }
                System::rm(array('-rf', $file));
            } elseif (file_exists($file)) {
                unlink($file);
            }
        }
    }

    // }}}
    // {{{ addTempFile()

    /**
     * Register a temporary file or directory.  When the destructor is
     * executed, all registered temporary files and directories are
     * removed.
     *
     * @param string  $file  name of file or directory
     *
     * @return void
     *
     * @access public
     */
    function addTempFile($file)
    {
        if (!class_exists('PEAR_Frontend')) {
            require_once 'PEAR/Frontend.php';
        }
        PEAR_Frontend::addTempFile($file);
    }

    // }}}
    // {{{ mkDirHier()

    /**
     * Wrapper to System::mkDir(), creates a directory as well as
     * any necessary parent directories.
     *
     * @param string  $dir  directory name
     *
     * @return bool TRUE on success, or a PEAR error
     *
     * @access public
     */
    function mkDirHier($dir)
    {
        $this->log(2, "+ create dir $dir");
        if (!class_exists('System')) {
            require_once 'System.php';
        }
        return System::mkDir(array('-p', $dir));
    }

    // }}}
    // {{{ log()

    /**
     * Logging method.
     *
     * @param int    $level  log level (0 is quiet, higher is noisier)
     * @param string $msg    message to write to the log
     *
     * @return void
     *
     * @access public
     * @static
     */
    function log($level, $msg, $append_crlf = true)
    {
        if ($this->debug >= $level) {
            if (!class_exists('PEAR_Frontend')) {
                require_once 'PEAR/Frontend.php';
            }
            $ui = &PEAR_Frontend::singleton();
            if (is_a($ui, 'PEAR_Frontend')) {
                $ui->log($msg, $append_crlf);
            } else {
                print "$msg\n";
            }
        }
    }

    // }}}
    // {{{ mkTempDir()

    /**
     * Create and register a temporary directory.
     *
     * @param string $tmpdir (optional) Directory to use as tmpdir.
     *                       Will use system defaults (for example
     *                       /tmp or c:\windows\temp) if not specified
     *
     * @return string name of created directory
     *
     * @access public
     */
    function mkTempDir($tmpdir = '')
    {
        if ($tmpdir) {
            $topt = array('-t', $tmpdir);
        } else {
            $topt = array();
        }
        $topt = array_merge($topt, array('-d', 'pear'));
        if (!class_exists('System')) {
            require_once 'System.php';
        }
        if (!$tmpdir = System::mktemp($topt)) {
            return false;
        }
        $this->addTempFile($tmpdir);
        return $tmpdir;
    }

    // }}}
    // {{{ setFrontendObject()

    /**
     * Set object that represents the frontend to be used.
     *
     * @param  object Reference of the frontend object
     * @return void
     * @access public
     */
    function setFrontendObject(&$ui)
    {
        $this->ui = &$ui;
    }

    // }}}

    // {{{ infoFromTgzFile()

    /**
     * Returns information about a package file.  Expects the name of
     * a gzipped tar file as input.
     *
     * @param string  $file  name of .tgz file
     *
     * @return array  array with package information
     *
     * @access public
     * @deprecated use PEAR_PackageFile->fromTgzFile() instead
     *
     */
    function infoFromTgzFile($file)
    {
        $packagefile = &new PEAR_PackageFile($this->config);
        $pf = &$packagefile->fromTgzFile($file, PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($pf)) {
            $errs = $pf->getUserinfo();
            if (is_array($errs)) {
                foreach ($errs as $error) {
                    $e = $this->raiseError($error['message'], $error['code'], null, null, $error);
                }
            }
            return $pf;
        }
        return $this->_postProcessValidPackagexml($pf);
    }

    // }}}
    // {{{ infoFromDescriptionFile()

    /**
     * Returns information about a package file.  Expects the name of
     * a package xml file as input.
     *
     * @param string  $descfile  name of package xml file
     *
     * @return array  array with package information
     *
     * @access public
     * @deprecated use PEAR_PackageFile->fromPackageFile() instead
     *
     */
    function infoFromDescriptionFile($descfile)
    {
        $packagefile = &new PEAR_PackageFile($this->config);
        $pf = &$packagefile->fromPackageFile($descfile, PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($pf)) {
            $errs = $pf->getUserinfo();
            if (is_array($errs)) {
                foreach ($errs as $error) {
                    $e = $this->raiseError($error['message'], $error['code'], null, null, $error);
                }
            }
            return $pf;
        }
        return $this->_postProcessValidPackagexml($pf);
    }

    // }}}
    // {{{ infoFromString()

    /**
     * Returns information about a package file.  Expects the contents
     * of a package xml file as input.
     *
     * @param string  $data  contents of package.xml file
     *
     * @return array   array with package information
     *
     * @access public
     * @deprecated use PEAR_PackageFile->fromXmlstring() instead
     *
     */
    function infoFromString($data)
    {
        $packagefile = &new PEAR_PackageFile($this->config);
        $pf = &$packagefile->fromXmlString($data, PEAR_VALIDATE_NORMAL, false);
        if (PEAR::isError($pf)) {
            $errs = $pf->getUserinfo();
            if (is_array($errs)) {
                foreach ($errs as $error) {
                    $e = $this->raiseError($error['message'], $error['code'], null, null, $error);
                }
            }
            return $pf;
        }
        return $this->_postProcessValidPackagexml($pf);
    }
    // }}}

    /**
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @return array
     */
    function _postProcessValidPackagexml(&$pf)
    {
        if (is_a($pf, 'PEAR_PackageFile_v2')) {
            // sort of make this into a package.xml 1.0-style array
            // changelog is not converted to old format.
            $arr = $pf->toArray(true);
            $arr = array_merge($arr, $arr['old']);
            unset($arr['old']);
            unset($arr['xsdversion']);
            unset($arr['contents']);
            unset($arr['compatible']);
            unset($arr['channel']);
            unset($arr['uri']);
            unset($arr['dependencies']);
            unset($arr['phprelease']);
            unset($arr['extsrcrelease']);
            unset($arr['extbinrelease']);
            unset($arr['bundle']);
            unset($arr['lead']);
            unset($arr['developer']);
            unset($arr['helper']);
            unset($arr['contributor']);
            $arr['filelist'] = $pf->getFilelist();
            $this->pkginfo = $arr;
            return $arr;
        } else {
            $this->pkginfo = $pf->toArray();
            return $this->pkginfo;
        }
    }
    // {{{ infoFromAny()

    /**
     * Returns package information from different sources
     *
     * This method is able to extract information about a package
     * from a .tgz archive or from a XML package definition file.
     *
     * @access public
     * @param  string Filename of the source ('package.xml', '<package>.tgz')
     * @return string
     * @deprecated use PEAR_PackageFile->fromAnyFile() instead
     */
    function infoFromAny($info)
    {
        if (is_string($info) && file_exists($info)) {
            $packagefile = &new PEAR_PackageFile($this->config);
            $pf = &$packagefile->fromAnyFile($info, PEAR_VALIDATE_NORMAL);
            if (PEAR::isError($pf)) {
                $errs = $pf->getUserinfo();
                if (is_array($errs)) {
                    foreach ($errs as $error) {
                        $e = $this->raiseError($error['message'], $error['code'], null, null, $error);
                    }
                }
                return $pf;
            }
            return $this->_postProcessValidPackagexml($pf);
        }
        return $info;
    }

    // }}}
    // {{{ xmlFromInfo()

    /**
     * Return an XML document based on the package info (as returned
     * by the PEAR_Common::infoFrom* methods).
     *
     * @param array  $pkginfo  package info
     *
     * @return string XML data
     *
     * @access public
     * @deprecated use a PEAR_PackageFile_v* object's generator instead
     */
    function xmlFromInfo($pkginfo)
    {
        $config = &PEAR_Config::singleton();
        $packagefile = &new PEAR_PackageFile($config);
        $pf = &$packagefile->fromArray($pkginfo);
        $gen = &$pf->getDefaultGenerator();
        return $gen->toXml(PEAR_VALIDATE_PACKAGING);
    }

    // }}}
    // {{{ validatePackageInfo()

    /**
     * Validate XML package definition file.
     *
     * @param  string $info Filename of the package archive or of the
     *                package definition file
     * @param  array $errors Array that will contain the errors
     * @param  array $warnings Array that will contain the warnings
     * @param  string $dir_prefix (optional) directory where source files
     *                may be found, or empty if they are not available
     * @access public
     * @return boolean
     * @deprecated use the validation of PEAR_PackageFile objects
     */
    function validatePackageInfo($info, &$errors, &$warnings, $dir_prefix = '')
    {
        $config = &PEAR_Config::singleton();
        $packagefile = &new PEAR_PackageFile($config);
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        if (strpos($info, '<?xml') !== false) {
            $pf = &$packagefile->fromXmlString($info, PEAR_VALIDATE_NORMAL, '');
        } else {
            $pf = &$packagefile->fromAnyFile($info, PEAR_VALIDATE_NORMAL);
        }
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($pf)) {
            $errs = $pf->getUserinfo();
            if (is_array($errs)) {
                foreach ($errs as $error) {
                    if ($error['level'] == 'error') {
                        $errors[] = $error['message'];
                    } else {
                        $warnings[] = $error['message'];
                    }
                }
            }
            return false;
        }
        return true;
    }

    // }}}
    // {{{ buildProvidesArray()

    /**
     * Build a "provides" array from data returned by
     * analyzeSourceCode().  The format of the built array is like
     * this:
     *
     *  array(
     *    'class;MyClass' => 'array('type' => 'class', 'name' => 'MyClass'),
     *    ...
     *  )
     *
     *
     * @param array $srcinfo array with information about a source file
     * as returned by the analyzeSourceCode() method.
     *
     * @return void
     *
     * @access public
     *
     */
    function buildProvidesArray($srcinfo)
    {
        $file = basename($srcinfo['source_file']);
        $pn = '';
        if (isset($this->_packageName)) {
            $pn = $this->_packageName;
        }
        $pnl = strlen($pn);
        foreach ($srcinfo['declared_classes'] as $class) {
            $key = "class;$class";
            if (isset($this->pkginfo['provides'][$key])) {
                continue;
            }
            $this->pkginfo['provides'][$key] =
                array('file'=> $file, 'type' => 'class', 'name' => $class);
            if (isset($srcinfo['inheritance'][$class])) {
                $this->pkginfo['provides'][$key]['extends'] =
                    $srcinfo['inheritance'][$class];
            }
        }
        foreach ($srcinfo['declared_methods'] as $class => $methods) {
            foreach ($methods as $method) {
                $function = "$class::$method";
                $key = "function;$function";
                if ($method{0} == '_' || !strcasecmp($method, $class) ||
                    isset($this->pkginfo['provides'][$key])) {
                    continue;
                }
                $this->pkginfo['provides'][$key] =
                    array('file'=> $file, 'type' => 'function', 'name' => $function);
            }
        }

        foreach ($srcinfo['declared_functions'] as $function) {
            $key = "function;$function";
            if ($function{0} == '_' || isset($this->pkginfo['provides'][$key])) {
                continue;
            }
            if (!strstr($function, '::') && strncasecmp($function, $pn, $pnl)) {
                $warnings[] = "in1 " . $file . ": function \"$function\" not prefixed with package name \"$pn\"";
            }
            $this->pkginfo['provides'][$key] =
                array('file'=> $file, 'type' => 'function', 'name' => $function);
        }
    }

    // }}}
    // {{{ analyzeSourceCode()

    /**
     * Analyze the source code of the given PHP file
     *
     * @param  string Filename of the PHP file
     * @return mixed
     * @access public
     */
    function analyzeSourceCode($file)
    {
        if (!function_exists("token_get_all")) {
            return false;
        }
        if (!defined('T_DOC_COMMENT')) {
            define('T_DOC_COMMENT', T_COMMENT);
        }
        if (!defined('T_INTERFACE')) {
            define('T_INTERFACE', -1);
        }
        if (!defined('T_IMPLEMENTS')) {
            define('T_IMPLEMENTS', -1);
        }
        if (!$fp = @fopen($file, "r")) {
            return false;
        }
        if (function_exists('file_get_contents')) {
            fclose($fp);
            $contents = file_get_contents($file);
        } else {
            $contents = fread($fp, filesize($file));
            fclose($fp);
        }
        $tokens = token_get_all($contents);
/*
        for ($i = 0; $i < sizeof($tokens); $i++) {
            @list($token, $data) = $tokens[$i];
            if (is_string($token)) {
                var_dump($token);
            } else {
                print token_name($token) . ' ';
                var_dump(rtrim($data));
            }
        }
*/
        $look_for = 0;
        $paren_level = 0;
        $bracket_level = 0;
        $brace_level = 0;
        $lastphpdoc = '';
        $current_class = '';
        $current_interface = '';
        $current_class_level = -1;
        $current_function = '';
        $current_function_level = -1;
        $declared_classes = array();
        $declared_interfaces = array();
        $declared_functions = array();
        $declared_methods = array();
        $used_classes = array();
        $used_functions = array();
        $extends = array();
        $implements = array();
        $nodeps = array();
        $inquote = false;
        $interface = false;
        for ($i = 0; $i < sizeof($tokens); $i++) {
            if (is_array($tokens[$i])) {
                list($token, $data) = $tokens[$i];
            } else {
                $token = $tokens[$i];
                $data = '';
            }
            if ($inquote) {
                if ($token != '"') {
                    continue;
                } else {
                    $inquote = false;
                    continue;
                }
            }
            switch ($token) {
                case T_WHITESPACE:
                    continue;
                case ';':
                    if ($interface) {
                        $current_function = '';
                        $current_function_level = -1;
                    }
                    break;
                case '"':
                    $inquote = true;
                    break;
                case T_CURLY_OPEN:
                case T_DOLLAR_OPEN_CURLY_BRACES:
                case '{': $brace_level++; continue 2;
                case '}':
                    $brace_level--;
                    if ($current_class_level == $brace_level) {
                        $current_class = '';
                        $current_class_level = -1;
                    }
                    if ($current_function_level == $brace_level) {
                        $current_function = '';
                        $current_function_level = -1;
                    }
                    continue 2;
                case '[': $bracket_level++; continue 2;
                case ']': $bracket_level--; continue 2;
                case '(': $paren_level++;   continue 2;
                case ')': $paren_level--;   continue 2;
                case T_INTERFACE:
                    $interface = true;
                case T_CLASS:
                    if (($current_class_level != -1) || ($current_function_level != -1)) {
                        PEAR::raiseError("Parser error: invalid PHP found in file \"$file\"",
                            PEAR_COMMON_ERROR_INVALIDPHP);
                        return false;
                    }
                case T_FUNCTION:
                case T_NEW:
                case T_EXTENDS:
                case T_IMPLEMENTS:
                    $look_for = $token;
                    continue 2;
                case T_STRING:
                    if (version_compare(zend_version(), '2.0', '<')) {
                        if (in_array(strtolower($data),
                            array('public', 'private', 'protected', 'abstract',
                                  'interface', 'implements', 'throw') 
                                 )) {
                            PEAR::raiseError('Error: PHP5 token encountered in ' . $file . 
                            'packaging should be done in PHP 5');
                            return false;
                        }
                    }
                    if ($look_for == T_CLASS) {
                        $current_class = $data;
                        $current_class_level = $brace_level;
                        $declared_classes[] = $current_class;
                    } elseif ($look_for == T_INTERFACE) {
                        $current_interface = $data;
                        $current_class_level = $brace_level;
                        $declared_interfaces[] = $current_interface;
                    } elseif ($look_for == T_IMPLEMENTS) {
                        $implements[$current_class] = $data;
                    } elseif ($look_for == T_EXTENDS) {
                        $extends[$current_class] = $data;
                    } elseif ($look_for == T_FUNCTION) {
                        if ($current_class) {
                            $current_function = "$current_class::$data";
                            $declared_methods[$current_class][] = $data;
                        } elseif ($current_interface) {
                            $current_function = "$current_interface::$data";
                            $declared_methods[$current_interface][] = $data;
                        } else {
                            $current_function = $data;
                            $declared_functions[] = $current_function;
                        }
                        $current_function_level = $brace_level;
                        $m = array();
                    } elseif ($look_for == T_NEW) {
                        $used_classes[$data] = true;
                    }
                    $look_for = 0;
                    continue 2;
                case T_VARIABLE:
                    $look_for = 0;
                    continue 2;
                case T_DOC_COMMENT:
                case T_COMMENT:
                    if (preg_match('!^/\*\*\s!', $data)) {
                        $lastphpdoc = $data;
                        if (preg_match_all('/@nodep\s+(\S+)/', $lastphpdoc, $m)) {
                            $nodeps = array_merge($nodeps, $m[1]);
                        }
                    }
                    continue 2;
                case T_DOUBLE_COLON:
                    if (!($tokens[$i - 1][0] == T_WHITESPACE || $tokens[$i - 1][0] == T_STRING)) {
                        PEAR::raiseError("Parser error: invalid PHP found in file \"$file\"",
                            PEAR_COMMON_ERROR_INVALIDPHP);
                        return false;
                    }
                    $class = $tokens[$i - 1][1];
                    if (strtolower($class) != 'parent') {
                        $used_classes[$class] = true;
                    }
                    continue 2;
            }
        }
        return array(
            "source_file" => $file,
            "declared_classes" => $declared_classes,
            "declared_interfaces" => $declared_interfaces,
            "declared_methods" => $declared_methods,
            "declared_functions" => $declared_functions,
            "used_classes" => array_diff(array_keys($used_classes), $nodeps),
            "inheritance" => $extends,
            "implements" => $implements,
            );
    }

    // }}}
    // {{{  betterStates()

    /**
     * Return an array containing all of the states that are more stable than
     * or equal to the passed in state
     *
     * @param string Release state
     * @param boolean Determines whether to include $state in the list
     * @return false|array False if $state is not a valid release state
     */
    function betterStates($state, $include = false)
    {
        static $states = array('snapshot', 'devel', 'alpha', 'beta', 'stable');
        $i = array_search($state, $states);
        if ($i === false) {
            return false;
        }
        if ($include) {
            $i--;
        }
        return array_slice($states, $i + 1);
    }

    // }}}
    // {{{ detectDependencies()

    function detectDependencies($any, $status_callback = null)
    {
        if (!function_exists("token_get_all")) {
            return false;
        }
        if (PEAR::isError($info = $this->infoFromAny($any))) {
            return $this->raiseError($info);
        }
        if (!is_array($info)) {
            return false;
        }
        $deps = array();
        $used_c = $decl_c = $decl_f = $decl_m = array();
        foreach ($info['filelist'] as $file => $fa) {
            $tmp = $this->analyzeSourceCode($file);
            $used_c = @array_merge($used_c, $tmp['used_classes']);
            $decl_c = @array_merge($decl_c, $tmp['declared_classes']);
            $decl_f = @array_merge($decl_f, $tmp['declared_functions']);
            $decl_m = @array_merge($decl_m, $tmp['declared_methods']);
            $inheri = @array_merge($inheri, $tmp['inheritance']);
        }
        $used_c = array_unique($used_c);
        $decl_c = array_unique($decl_c);
        $undecl_c = array_diff($used_c, $decl_c);
        return array('used_classes' => $used_c,
                     'declared_classes' => $decl_c,
                     'declared_methods' => $decl_m,
                     'declared_functions' => $decl_f,
                     'undeclared_classes' => $undecl_c,
                     'inheritance' => $inheri,
                     );
    }

    // }}}
    // {{{ getUserRoles()

    /**
     * Get the valid roles for a PEAR package maintainer
     *
     * @return array
     * @static
     */
    function getUserRoles()
    {
        return $GLOBALS['_PEAR_Common_maintainer_roles'];
    }

    // }}}
    // {{{ getReleaseStates()

    /**
     * Get the valid package release states of packages
     *
     * @return array
     * @static
     */
    function getReleaseStates()
    {
        return $GLOBALS['_PEAR_Common_release_states'];
    }

    // }}}
    // {{{ getDependencyTypes()

    /**
     * Get the implemented dependency types (php, ext, pkg etc.)
     *
     * @return array
     * @static
     */
    function getDependencyTypes()
    {
        return $GLOBALS['_PEAR_Common_dependency_types'];
    }

    // }}}
    // {{{ getDependencyRelations()

    /**
     * Get the implemented dependency relations (has, lt, ge etc.)
     *
     * @return array
     * @static
     */
    function getDependencyRelations()
    {
        return $GLOBALS['_PEAR_Common_dependency_relations'];
    }

    // }}}
    // {{{ getFileRoles()

    /**
     * Get the implemented file roles
     *
     * @return array
     * @static
     */
    function getFileRoles()
    {
        return $GLOBALS['_PEAR_Common_file_roles'];
    }

    // }}}
    // {{{ getReplacementTypes()

    /**
     * Get the implemented file replacement types in
     *
     * @return array
     * @static
     */
    function getReplacementTypes()
    {
        return $GLOBALS['_PEAR_Common_replacement_types'];
    }

    // }}}
    // {{{ getProvideTypes()

    /**
     * Get the implemented file replacement types in
     *
     * @return array
     * @static
     */
    function getProvideTypes()
    {
        return $GLOBALS['_PEAR_Common_provide_types'];
    }

    // }}}
    // {{{ getScriptPhases()

    /**
     * Get the implemented file replacement types in
     *
     * @return array
     * @static
     */
    function getScriptPhases()
    {
        return $GLOBALS['_PEAR_Common_script_phases'];
    }

    // }}}
    // {{{ validPackageName()

    /**
     * Test whether a string contains a valid package name.
     *
     * @param string $name the package name to test
     *
     * @return bool
     *
     * @access public
     */
    function validPackageName($name)
    {
        return (bool)preg_match(PEAR_COMMON_PACKAGE_NAME_PREG, $name);
    }


    // }}}
    // {{{ validPackageVersion()

    /**
     * Test whether a string contains a valid package version.
     *
     * @param string $ver the package version to test
     *
     * @return bool
     *
     * @access public
     */
    function validPackageVersion($ver)
    {
        return (bool)preg_match(PEAR_COMMON_PACKAGE_VERSION_PREG, $ver);
    }


    // }}}

    // {{{ downloadHttp()

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
     *  'connfailed'  if the TCP connection fails, this callback is called
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
     * @param string  $save_dir  (optional) directory to save file in
     * @param mixed   $callback  (optional) function/method to call for status
     *                           updates
     *
     * @return string  Returns the full path of the downloaded file or a PEAR
     *                 error on failure.  If the error is caused by
     *                 socket-related errors, the error object will
     *                 have the fsockopen error code available through
     *                 getCode().
     *
     * @access public
     * @deprecated in favor of PEAR_Downloader::downloadHttp()
     */
    function downloadHttp($url, &$ui, $save_dir = '.', $callback = null)
    {
        if (!class_exists('PEAR_Downloader')) {
            require_once 'PEAR/Downloader.php';
        }
        return PEAR_Downloader::downloadHttp($url, $ui, $save_dir, $callback);
    }

    // }}}

    /**
     * @param string $path relative or absolute include path
     * @return boolean
     * @static
     */
    function isIncludeable($path)
    {
        if (file_exists($path) && is_readable($path)) {
            return true;
        }
        $ipath = explode(PATH_SEPARATOR, ini_get('include_path'));
        foreach ($ipath as $include) {
            $test = realpath($include . DIRECTORY_SEPARATOR . $path);
            if (file_exists($test) && is_readable($test)) {
                return true;
            }
        }
        return false;
    }
}
require_once 'PEAR/Config.php';
require_once 'PEAR/PackageFile.php';
?>