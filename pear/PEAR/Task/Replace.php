<?php
/**
 * <tasks:replace>
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
 * Base class
 */
require_once 'PEAR/Task/Common.php';
/**
 * Implements the replace file task.
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_Task_Replace extends PEAR_Task_Common
{
    var $type = 'simple';
    var $phase = PEAR_TASK_PACKAGEANDINSTALL;
    var $_replacements;

    /**
     * Validate the raw xml at parsing-time.
     * @param PEAR_PackageFile_v2
     * @param array raw, parsed xml
     * @param PEAR_Config
     * @static
     */
    function validateXml($pkg, $xml, &$config, $fileXml)
    {
        if (!isset($xml['attribs'])) {
            return array(PEAR_TASK_ERROR_NOATTRIBS);
        }
        if (!isset($xml['attribs']['type'])) {
            return array(PEAR_TASK_ERROR_MISSING_ATTRIB, 'type');
        }
        if (!isset($xml['attribs']['to'])) {
            return array(PEAR_TASK_ERROR_MISSING_ATTRIB, 'to');
        }
        if (!isset($xml['attribs']['from'])) {
            return array(PEAR_TASK_ERROR_MISSING_ATTRIB, 'from');
        }
        if ($xml['attribs']['type'] == 'pear-config') {
            if (!in_array($xml['attribs']['to'], $config->getKeys())) {
                return array(PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE, 'to', $xml['attribs']['to'],
                    $config->getKeys());
            }
        } elseif ($xml['attribs']['type'] == 'php-const') {
            if (defined($xml['attribs']['to'])) {
                return true;
            } else {
                return array(PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE, 'to', $xml['attribs']['to'],
                    array('valid PHP constant'));
            }
        } elseif ($xml['attribs']['type'] == 'package-info') {
            if (in_array($xml['attribs']['to'],
                array('name', 'summary', 'channel', 'notes', 'extends', 'description',
                    'release_notes', 'license', 'release-license', 'license-uri',
                    'version', 'api-version', 'state', 'api-state', 'release_date',
                    'date', 'time'))) {
                return true;
            } else {
                return array(PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE, 'to', $xml['attribs']['to'],
                    array('name', 'summary', 'channel', 'notes', 'extends', 'description',
                    'release_notes', 'license', 'release-license', 'license-uri',
                    'version', 'api-version', 'state', 'api-state', 'release_date',
                    'date', 'time'));
            }
        } else {
            return array(PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE, 'type', $xml['attribs']['type'],
                array('pear-config', 'package-info', 'php-const'));
        }
        return true;
    }

    /**
     * Initialize a task instance with the parameters
     * @param array raw, parsed xml
     * @param unused
     */
    function init($xml, $attribs)
    {
        $this->_replacements = isset($xml['attribs']) ? array($xml) : $xml;
    }

    /**
     * Do a package.xml 1.0 replacement, with additional package-info fields available
     *
     * See validateXml() source for the complete list of allowed fields
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @param string file contents
     * @param string the eventual final file location (informational only)
     * @return string|false|PEAR_Error false to skip this file, PEAR_Error to fail
     *         (use $this->throwError), otherwise return the new contents
     */
    function startSession($pkg, $contents, $dest)
    {
        $subst_from = $subst_to = array();
        foreach ($this->_replacements as $a) {
            $a = $a['attribs'];
            $to = '';
            if ($a['type'] == 'pear-config') {
                if ($this->installphase == PEAR_TASK_PACKAGE) {
                    return false;
                }
                if ($a['to'] == 'master_server') {
                    $chan = $this->registry->getChannel($pkg->getChannel());
                    if (!PEAR::isError($chan)) {
                        $to = $chan->getServer();
                    } else {
                        $this->logger->log(0, "$dest: invalid pear-config replacement: $a[to]");
                        return false;
                    }
                } else {
                    if ($this->config->isDefinedLayer('ftp')) {
                        // try the remote config file first
                        $to = $this->config->get($a['to'], 'ftp', $pkg->getChannel());
                        if (is_null($to)) {
                            // then default to local
                            $to = $this->config->get($a['to'], null, $pkg->getChannel());
                        }
                    } else {
                        $to = $this->config->get($a['to'], null, $pkg->getChannel());
                    }
                }
                if (is_null($to)) {
                    $this->logger->log(0, "$dest: invalid pear-config replacement: $a[to]");
                    return false;
                }
            } elseif ($a['type'] == 'php-const') {
                if ($this->installphase == PEAR_TASK_PACKAGE) {
                    return false;
                }
                if (defined($a['to'])) {
                    $to = constant($a['to']);
                } else {
                    $this->logger->log(0, "$dest: invalid php-const replacement: $a[to]");
                    return false;
                }
            } else {
                if ($t = $pkg->packageInfo($a['to'])) {
                    $to = $t;
                } else {
                    $this->logger->log(0, "$dest: invalid package-info replacement: $a[to]");
                    return false;
                }
            }
            if (!is_null($to)) {
                $subst_from[] = $a['from'];
                $subst_to[] = $to;
            }
        }
        $this->logger->log(3, "doing " . sizeof($subst_from) .
            " substitution(s) for $dest");
        if (sizeof($subst_from)) {
            $contents = str_replace($subst_from, $subst_to, $contents);
        }
        return $contents;
    }
}
?>