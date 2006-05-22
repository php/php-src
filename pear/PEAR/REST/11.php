<?php
/**
 * PEAR_REST_11 - implement faster list-all/remote-list command
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
 * @since      File available since Release 1.4.3
 */

/**
 * For downloading REST xml/txt files
 */
require_once 'PEAR/REST.php';

/**
 * Implement REST 1.1
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.3
 */
class PEAR_REST_11
{
    /**
     * @var PEAR_REST
     */
    var $_rest;

    function PEAR_REST_11($config, $options = array())
    {
        $this->_rest = &new PEAR_REST($config, $options);
    }

    function listAll($base, $dostable, $basic = true)
    {
        $categorylist = $this->_rest->retrieveData($base . 'c/categories.xml');
        if (PEAR::isError($categorylist)) {
            return $categorylist;
        }
        $ret = array();
        if (!is_array($categorylist['c'])) {
            $categorylist['c'] = array($categorylist['c']);
        }
        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
        foreach ($categorylist['c'] as $progress => $category) {
            $category = $category['_content'];
            $packagesinfo = $this->_rest->retrieveData($base .
                'c/' . urlencode($category) . '/packagesinfo.xml');
            if (PEAR::isError($packagesinfo)) {
                continue;
            }
            if (!is_array($packagesinfo) || !isset($packagesinfo['pi'])) {
                continue;
            }
            if (!is_array($packagesinfo['pi']) || !isset($packagesinfo['pi'][0])) {
                $packagesinfo['pi'] = array($packagesinfo['pi']);
            }
            foreach ($packagesinfo['pi'] as $packageinfo) {
                $info = $packageinfo['p'];
                $package = $info['n'];
                $releases = isset($packageinfo['a']) ? $packageinfo['a'] : false;
                unset($latest);
                unset($unstable);
                unset($stable);
                unset($state);
                if ($releases) {
                    if (!isset($releases['r'][0])) {
                        $releases['r'] = array($releases['r']);
                    }
                    foreach ($releases['r'] as $release) {
                        if (!isset($latest)) {
                            if ($dostable && $release['s'] == 'stable') {
                                $latest = $release['v'];
                                $state = 'stable';
                            }
                            if (!$dostable) {
                                $latest = $release['v'];
                                $state = $release['s'];
                            }
                        }
                        if (!isset($stable) && $release['s'] == 'stable') {
                            $stable = $release['v'];
                            if (!isset($unstable)) {
                                $unstable = $stable;
                            }
                        }
                        if (!isset($unstable) && $release['s'] != 'stable') {
                            $latest = $unstable = $release['v'];
                            $state = $release['s'];
                        }
                        if (isset($latest) && !isset($state)) {
                            $state = $release['s'];
                        }
                        if (isset($latest) && isset($stable) && isset($unstable)) {
                            break;
                        }
                    }
                }
                if ($basic) { // remote-list command
                    if (!isset($latest)) {
                        $latest = false;
                    }
                    $ret[$package] = array('stable' => $latest);
                    continue;
                }
                // list-all command
                $deps = array();
                if (!isset($unstable)) {
                    $unstable = false;
                    $state = 'stable';
                    if (isset($stable)) {
                        $latest = $unstable = $stable;
                    }
                } else {
                    $latest = $unstable;
                }
                if (!isset($latest)) {
                    $latest = false;
                }
                if ($latest) {
                    if (isset($packageinfo['deps'])) {
                        if (!is_array($packageinfo['deps']) ||
                              !isset($packageinfo['deps'][0])) {
                            $packageinfo['deps'] = array($packageinfo['deps']);
                        }
                    }
                    $d = false;
                    foreach ($packageinfo['deps'] as $dep) {
                        if ($dep['v'] == $latest) {
                            $d = unserialize($dep['d']);
                        }
                    }
                    if ($d) {
                        if (isset($d['required'])) {
                            if (!class_exists('PEAR_PackageFile_v2')) {
                                require_once 'PEAR/PackageFile/v2.php';
                            }
                            if (!isset($pf)) {
                                $pf = new PEAR_PackageFile_v2;
                            }
                            $pf->setDeps($d);
                            $tdeps = $pf->getDeps();
                        } else {
                            $tdeps = $d;
                        }
                        foreach ($tdeps as $dep) {
                            if ($dep['type'] !== 'pkg') {
                                continue;
                            }
                            $deps[] = $dep;
                        }
                    }
                }
                if (!isset($stable)) {
                    $stable = '-n/a-';
                }
                $info = array('stable' => $latest, 'summary' => $info['s'],
                    'description' =>
                    $info['d'], 'deps' => $deps, 'category' => $info['ca']['_content'],
                    'unstable' => $unstable, 'state' => $state);
                $ret[$package] = $info;
            }
        }
        PEAR::popErrorHandling();
        return $ret;
    }

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
}
?>