<?php
/**
 * PEAR_Validate
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
/**#@+
 * Constants for install stage
 */
define('PEAR_VALIDATE_INSTALLING', 1);
define('PEAR_VALIDATE_UNINSTALLING', 2); // this is not bit-mapped like the others
define('PEAR_VALIDATE_NORMAL', 3);
define('PEAR_VALIDATE_DOWNLOADING', 4); // this is not bit-mapped like the others
define('PEAR_VALIDATE_PACKAGING', 7);
/**#@-*/
require_once 'PEAR/Common.php';
require_once 'PEAR/Validator/PECL.php';

/**
 * Validation class for package.xml - channel-level advanced validation
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_Validate
{
    var $packageregex = _PEAR_COMMON_PACKAGE_NAME_PREG;
    /**
     * @var PEAR_PackageFile_v1|PEAR_PackageFile_v2
     */
    var $_packagexml;
    /**
     * @var int one of the PEAR_VALIDATE_* constants
     */
    var $_state = PEAR_VALIDATE_NORMAL;
    /**
     * Format: ('error' => array('field' => name, 'reason' => reason), 'warning' => same)
     * @var array
     * @access private
     */
    var $_failures = array('error' => array(), 'warning' => array());

    /**
     * Override this method to handle validation of normal package names
     * @param string
     * @return bool
     * @access protected
     */
    function _validPackageName($name)
    {
        return (bool) preg_match('/^' . $this->packageregex . '$/', $name);
    }

    /**
     * @param string package name to validate
     * @param string name of channel-specific validation package
     * @final
     */
    function validPackageName($name, $validatepackagename = false)
    {
        if ($validatepackagename) {
            if (strtolower($name) == strtolower($validatepackagename)) {
                return (bool) preg_match('/^[a-zA-Z0-9_]+(?:\.[a-zA-Z0-9_]+)*$/', $name);
            }
        }
        return $this->_validPackageName($name);
    }

    /**
     * This validates a bundle name, and bundle names must conform
     * to the PEAR naming convention, so the method is final and static.
     * @param string
     * @final
     * @static
     */
    function validGroupName($name)
    {
        return (bool) preg_match('/^' . _PEAR_COMMON_PACKAGE_NAME_PREG . '$/', $name);
    }

    /**
     * Determine whether $state represents a valid stability level
     * @param string
     * @return bool
     * @static
     * @final
     */
    function validState($state)
    {
        return in_array($state, array('snapshot', 'devel', 'alpha', 'beta', 'stable'));
    }

    /**
     * Get a list of valid stability levels
     * @return array
     * @static
     * @final
     */
    function getValidStates()
    {
        return array('snapshot', 'devel', 'alpha', 'beta', 'stable');
    }

    /**
     * Determine whether a version is a properly formatted version number that can be used
     * by version_compare
     * @param string
     * @return bool
     * @static
     * @final
     */
    function validVersion($ver)
    {
        return (bool) preg_match(PEAR_COMMON_PACKAGE_VERSION_PREG, $ver);
    }

    /**
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     */
    function setPackageFile(&$pf)
    {
        $this->_packagexml = &$pf;
    }

    /**
     * @access private
     */
    function _addFailure($field, $reason)
    {
        $this->_failures['errors'][] = array('field' => $field, 'reason' => $reason);
    }

    /**
     * @access private
     */
    function _addWarning($field, $reason)
    {
        $this->_failures['warnings'][] = array('field' => $field, 'reason' => $reason);
    }

    function getFailures()
    {
        $failures = $this->_failures;
        $this->_failures = array('warnings' => array(), 'errors' => array());
        return $failures;
    }

    /**
     * @param int one of the PEAR_VALIDATE_* constants
     */
    function validate($state = null)
    {
        if (!isset($this->_packagexml)) {
            return false;
        }
        if ($state !== null) {
            $this->_state = $state;
        }
        $this->_failures = array('warnings' => array(), 'errors' => array());
        $this->validatePackageName();
        $this->validateVersion();
        $this->validateMaintainers();
        $this->validateDate();
        $this->validateSummary();
        $this->validateDescription();
        $this->validateLicense();
        $this->validateNotes();
        if ($this->_packagexml->getPackagexmlVersion() == '1.0') {
            $this->validateState();
            $this->validateFilelist();
        } elseif ($this->_packagexml->getPackagexmlVersion() == '2.0') {
            $this->validateTime();
            $this->validateStability();
            $this->validateDeps();
            $this->validateMainFilelist();
            $this->validateReleaseFilelist();
            //$this->validateGlobalTasks();
            $this->validateChangelog();
        }
        return !((bool) count($this->_failures['errors']));
    }

    /**
     * @access protected
     */
    function validatePackageName()
    {
        if ($this->_state == PEAR_VALIDATE_PACKAGING ||
              $this->_state == PEAR_VALIDATE_NORMAL) {
            if ($this->_packagexml->getPackagexmlVersion() == '2.0' &&
                  $this->_packagexml->getExtends()) {
                $version = $this->_packagexml->getVersion() . '';
                $name = $this->_packagexml->getPackage();
                $test = array_shift($a = explode('.', $version));
                if ($test == '0') {
                    return true;
                }
                $vlen = strlen($test);
                $majver = substr($name, strlen($name) - $vlen);
                while ($majver && !is_numeric($majver{0})) {
                    $majver = substr($majver, 1);
                }
                if ($majver != $test) {
                    $this->_addWarning('package', "package $name extends package " .
                        $this->_packagexml->getExtends() . ' and so the name should ' .
                        'have a postfix equal to the major version like "' .
                        $this->_packagexml->getExtends() . $test . '"');
                    return true;
                } elseif (substr($name, 0, strlen($name) - $vlen) !=
                            $this->_packagexml->getExtends()) {
                    $this->_addWarning('package', "package $name extends package " .
                        $this->_packagexml->getExtends() . ' and so the name must ' .
                        'be an extension like "' . $this->_packagexml->getExtends() .
                        $test . '"');
                    return true;
                }
            }
        }
        if (!$this->validPackageName($this->_packagexml->getPackage())) {
            $this->_addFailure('name', 'package name "' .
                $this->_packagexml->getPackage() . '" is invalid');
            return false;
        }
    }

    /**
     * @access protected
     */
    function validateVersion()
    {
        if ($this->_state != PEAR_VALIDATE_PACKAGING) {
            if (!$this->validVersion($this->_packagexml->getVersion())) {
                $this->_addFailure('version',
                    'Invalid version number "' . $this->_packagexml->getVersion() . '"');
            }
            return false;
        }
        $version = $this->_packagexml->getVersion();
        $versioncomponents = explode('.', $version);
        if (count($versioncomponents) != 3) {
            $this->_addWarning('version',
                'A version number should have 3 decimals (x.y.z)');
            return true;
        }
        $name = $this->_packagexml->getPackage();
        // version must be based upon state
        switch ($this->_packagexml->getState()) {
            case 'snapshot' :
                return true;
            case 'devel' :
                if ($versioncomponents[0] . 'a' == '0a') {
                    return true;
                }
                if ($versioncomponents[0] == 0) {
                    $versioncomponents[0] = '0';
                    $this->_addWarning('version',
                        'version "' . $version . '" should be "' .
                        implode('.' ,$versioncomponents) . '"');
                } else {
                    $this->_addWarning('version',
                        'packages with devel stability must be < version 1.0.0');
                }
                return true;
            break;
            case 'alpha' :
            case 'beta' :
                // check for a package that extends a package,
                // like Foo and Foo2
                if ($this->_state == PEAR_VALIDATE_PACKAGING) {
                    if (substr($versioncomponents[2], 1, 2) == 'rc') {
                        $this->_addFailure('version', 'Release Candidate versions ' .
                            'must have capital RC, not lower-case rc');
                        return false;
                    }
                }
                if (!$this->_packagexml->getExtends()) {
                    if ($versioncomponents[0] == '1') {
                        if ($versioncomponents[2]{0} == '0') {
                            if ($versioncomponents[2] == '0') {
                                // version 1.*.0000
                                $this->_addWarning('version',
                                    'version 1.' . $versioncomponents[1] .
                                        '.0 probably should not be alpha or beta');
                                return true;
                            } elseif (strlen($versioncomponents[2]) > 1) {
                                // version 1.*.0RC1 or 1.*.0beta24 etc.
                                return true;
                            } else {
                                // version 1.*.0
                                $this->_addWarning('version',
                                    'version 1.' . $versioncomponents[1] .
                                        '.0 probably should not be alpha or beta');
                                return true;
                            }
                        } else {
                            $this->_addWarning('version',
                                'bugfix versions (1.3.x where x > 0) probably should ' .
                                'not be alpha or beta');
                            return true;
                        }
                    } elseif ($versioncomponents[0] != '0') {
                        $this->_addWarning('version',
                            'major versions greater than 1 are not allowed for packages ' .
                            'without an <extends> tag or an identical postfix (foo2 v2.0.0)');
                        return true;
                    }
                    if ($versioncomponents[0] . 'a' == '0a') {
                        return true;
                    }
                    if ($versioncomponents[0] == 0) {
                        $versioncomponents[0] = '0';
                        $this->_addWarning('version',
                            'version "' . $version . '" should be "' .
                            implode('.' ,$versioncomponents) . '"');
                    }
                } else {
                    $vlen = strlen($versioncomponents[0] . '');
                    $majver = substr($name, strlen($name) - $vlen);
                    while ($majver && !is_numeric($majver{0})) {
                        $majver = substr($majver, 1);
                    }
                    if (($versioncomponents[0] != 0) && $majver != $versioncomponents[0]) {
                        $this->_addWarning('version', 'first version number "' .
                            $versioncomponents[0] . '" must match the postfix of ' .
                            'package name "' . $name . '" (' .
                            $majver . ')');
                        return true;
                    }
                    if ($versioncomponents[0] == $majver) {
                        if ($versioncomponents[2]{0} == '0') {
                            if ($versioncomponents[2] == '0') {
                                // version 2.*.0000
                                $this->_addWarning('version',
                                    "version $majver." . $versioncomponents[1] .
                                        '.0 probably should not be alpha or beta');
                                return false;
                            } elseif (strlen($versioncomponents[2]) > 1) {
                                // version 2.*.0RC1 or 2.*.0beta24 etc.
                                return true;
                            } else {
                                // version 2.*.0
                                $this->_addWarning('version',
                                    "version $majver." . $versioncomponents[1] .
                                        '.0 cannot be alpha or beta');
                                return true;
                            }
                        } else {
                            $this->_addWarning('version',
                                "bugfix versions ($majver.x.y where y > 0) should " .
                                'not be alpha or beta');
                            return true;
                        }
                    } elseif ($versioncomponents[0] != '0') {
                        $this->_addWarning('version',
                            "only versions 0.x.y and $majver.x.y are allowed for alpha/beta releases");
                        return true;
                    }
                    if ($versioncomponents[0] . 'a' == '0a') {
                        return true;
                    }
                    if ($versioncomponents[0] == 0) {
                        $versioncomponents[0] = '0';
                        $this->_addWarning('version',
                            'version "' . $version . '" should be "' .
                            implode('.' ,$versioncomponents) . '"');
                    }
                }
                return true;
            break;
            case 'stable' :
                if ($versioncomponents[0] == '0') {
                    $this->_addWarning('version', 'versions less than 1.0.0 cannot ' .
                    'be stable');
                    return true;
                }
                if (!is_numeric($versioncomponents[2])) {
                    if (preg_match('/\d+(rc|a|alpha|b|beta)\d*/i',
                          $versioncomponents[2])) {
                        $this->_addWarning('version', 'version "' . $version . '" or any ' .
                            'RC/beta/alpha version cannot be stable');
                        return true;
                    }
                }
                // check for a package that extends a package,
                // like Foo and Foo2
                if ($this->_packagexml->getExtends()) {
                    $vlen = strlen($versioncomponents[0] . '');
                    $majver = substr($name, strlen($name) - $vlen);
                    while ($majver && !is_numeric($majver{0})) {
                        $majver = substr($majver, 1);
                    }
                    if (($versioncomponents[0] != 0) && $majver != $versioncomponents[0]) {
                        $this->_addWarning('version', 'first version number "' .
                            $versioncomponents[0] . '" must match the postfix of ' .
                            'package name "' . $name . '" (' .
                            $majver . ')');
                        return true;
                    }
                } elseif ($versioncomponents[0] > 1) {
                    $this->_addWarning('version', 'major version x in x.y.z may not be greater than ' .
                        '1 for any package that does not have an <extends> tag');
                }
                return true;
            break;
            default :
                return false;
            break;
        }
    }

    /**
     * @access protected
     */
    function validateMaintainers()
    {
        // maintainers can only be truly validated server-side for most channels
        // but allow this customization for those who wish it
        return true;
    }

    /**
     * @access protected
     */
    function validateDate()
    {
        if ($this->_state == PEAR_VALIDATE_NORMAL ||
              $this->_state == PEAR_VALIDATE_PACKAGING) {
            if (!preg_match('/\d\d\d\d\-\d\d\-\d\d/',
                  $this->_packagexml->getDate())) {
                $this->_addFailure('date', 'invalid release date "' .
                    $this->_packagexml->getDate() . '"');
                return false;
            }
            if (strtotime($this->_packagexml->getDate()) == -1) {
                $this->_addFailure('date', 'invalid release date "' .
                    $this->_packagexml->getDate() . '"');
                return false;
            }
            if ($this->_state == PEAR_VALIDATE_PACKAGING &&
                  $this->_packagexml->getDate() != date('Y-m-d')) {
                $this->_addWarning('date', 'Release Date "' .
                    $this->_packagexml->getDate() . '" is not today');
            }
        }
        return true;
    }

    /**
     * @access protected
     */
    function validateTime()
    {
        if (!$this->_packagexml->getTime()) {
            // default of no time value set
            return true;
        }
        // packager automatically sets time, so only validate if
        // pear validate is called
        if ($this->_state = PEAR_VALIDATE_NORMAL) {
            if (!preg_match('/\d\d:\d\d:\d\d/',
                  $this->_packagexml->getTime())) {
                $this->_addFailure('time', 'invalid release time "' .
                    $this->_packagexml->getTime() . '"');
                return false;
            }
            if (strtotime($this->_packagexml->getTime()) == -1) {
                $this->_addFailure('time', 'invalid release time "' .
                    $this->_packagexml->getTime() . '"');
                return false;
            }
        }
        return true;
    }

    /**
     * @access protected
     */
    function validateState()
    {
        // this is the closest to "final" php4 can get
        if (!PEAR_Validate::validState($this->_packagexml->getState())) {
            if (strtolower($this->_packagexml->getState() == 'rc')) {
                $this->_addFailure('state', 'RC is not a state, it is a version ' .
                    'postfix, use ' . $this->_packagexml->getVersion() . 'RC1, state beta');
            }
            $this->_addFailure('state', 'invalid release state "' .
                $this->_packagexml->getState() . '", must be one of: ' .
                implode(', ', PEAR_Validate::getValidStates()));
            return false;
        }
        return true;
    }

    /**
     * @access protected
     */
    function validateStability()
    {
        $ret = true;
        $packagestability = $this->_packagexml->getState();
        $apistability = $this->_packagexml->getState('api');
        if (!PEAR_Validate::validState($packagestability)) {
            $this->_addFailure('state', 'invalid release stability "' .
                $this->_packagexml->getState() . '", must be one of: ' .
                implode(', ', PEAR_Validate::getValidStates()));
            $ret = false;
        }
        $apistates = PEAR_Validate::getValidStates();
        array_shift($apistates); // snapshot is not allowed
        if (!in_array($apistability, $apistates)) {
            $this->_addFailure('state', 'invalid API stability "' .
                $this->_packagexml->getState('api') . '", must be one of: ' .
                implode(', ', $apistates));
            $ret = false;
        }
        return $ret;
    }

    /**
     * @access protected
     */
    function validateSummary()
    {
        return true;
    }

    /**
     * @access protected
     */
    function validateDescription()
    {
        return true;
    }

    /**
     * @access protected
     */
    function validateLicense()
    {
        return true;
    }

    /**
     * @access protected
     */
    function validateNotes()
    {
        return true;
    }

    /**
     * for package.xml 2.0 only - channels can't use package.xml 1.0
     * @access protected
     */
    function validateDependencies()
    {
        return true;
    }

    /**
     * for package.xml 1.0 only
     * @access private
     */
    function _validateFilelist()
    {
        return true; // placeholder for now
    }

    /**
     * for package.xml 2.0 only
     * @access protected
     */
    function validateMainFilelist()
    {
        return true; // placeholder for now
    }

    /**
     * for package.xml 2.0 only
     * @access protected
     */
    function validateReleaseFilelist()
    {
        return true; // placeholder for now
    }

    /**
     * @access protected
     */
    function validateChangelog()
    {
        return true;
    }

    /**
     * @access protected
     */
    function validateFilelist()
    {
        return true;
    }

    /**
     * @access protected
     */
    function validateDeps()
    {
        return true;
    }
}
?>