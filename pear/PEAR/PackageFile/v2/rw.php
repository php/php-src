<?php
/**
 * PEAR_PackageFile_v2, package.xml version 2.0, read/write version
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
 * @since      File available since Release 1.4.0a8
 */
/**
 * For base class
 */
require_once 'PEAR/PackageFile/v2.php';
/**
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a8
 */
class PEAR_PackageFile_v2_rw extends PEAR_PackageFile_v2
{
    /**
     * @param string Extension name
     * @return bool success of operation
     */
    function setProvidesExtension($extension)
    {
        if (in_array($this->getPackageType(), array('extsrc', 'extbin'))) {
            if (!isset($this->_packageInfo['providesextension'])) {
                // ensure that the channel tag is set up in the right location
                $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                    array('usesrole', 'usestask', 'srcpackage', 'srcuri', 'phprelease',
                    'extsrcrelease', 'extbinrelease', 'bundle', 'changelog'),
                    $extension, 'providesextension');
            }
            $this->_packageInfo['providesextension'] = $extension;
            return true;
        }
        return false;
    }

    function setPackage($package)
    {
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['attribs'])) {
            $this->_packageInfo = array_merge(array('attribs' => array(
                                 'version' => '2.0',
                                 'xmlns' => 'http://pear.php.net/dtd/package-2.0',
                                 'xmlns:tasks' => 'http://pear.php.net/dtd/tasks-1.0',
                                 'xmlns:xsi' => 'http://www.w3.org/2001/XMLSchema-instance',
                                 'xsi:schemaLocation' => 'http://pear.php.net/dtd/tasks-1.0
    http://pear.php.net/dtd/tasks-1.0.xsd
    http://pear.php.net/dtd/package-2.0
    http://pear.php.net/dtd/package-2.0.xsd',
                             )), $this->_packageInfo);
        }
        if (!isset($this->_packageInfo['name'])) {
            return $this->_packageInfo = array_merge(array('name' => $package),
                $this->_packageInfo);
        }
        $this->_packageInfo['name'] = $package;
    }

    function setUri($uri)
    {
        unset($this->_packageInfo['channel']);
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['uri'])) {
            // ensure that the uri tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo, 
                array('extends', 'summary', 'description', 'lead',
                'developer', 'contributor', 'helper', 'date', 'time', 'version',
                'stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), $uri, 'uri');
        }
        $this->_packageInfo['uri'] = $uri;
    }

    function setChannel($channel)
    {
        unset($this->_packageInfo['uri']);
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['channel'])) {
            // ensure that the channel tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('extends', 'summary', 'description', 'lead',
                'developer', 'contributor', 'helper', 'date', 'time', 'version',
                'stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), $channel, 'channel');
        }
        $this->_packageInfo['channel'] = $channel;
    }

    function setExtends($extends)
    {
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['extends'])) {
            // ensure that the extends tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('summary', 'description', 'lead',
                'developer', 'contributor', 'helper', 'date', 'time', 'version',
                'stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), $extends, 'extends');
        }
        $this->_packageInfo['extends'] = $extends;
    }

    function setSummary($summary)
    {
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['summary'])) {
            // ensure that the summary tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('description', 'lead',
                'developer', 'contributor', 'helper', 'date', 'time', 'version',
                'stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), $summary, 'summary');
        }
        $this->_packageInfo['summary'] = $summary;
    }

    function setDescription($desc)
    {
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['description'])) {
            // ensure that the description tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('lead',
                'developer', 'contributor', 'helper', 'date', 'time', 'version',
                'stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), $desc, 'description');
        }
        $this->_packageInfo['description'] = $desc;
    }

    /**
     * Adds a new maintainer - no checking of duplicates is performed, use
     * updatemaintainer for that purpose.
     */
    function addMaintainer($role, $handle, $name, $email, $active = 'yes')
    {
        if (!in_array($role, array('lead', 'developer', 'contributor', 'helper'))) {
            return false;
        }
        if (isset($this->_packageInfo[$role])) {
            if (!isset($this->_packageInfo[$role][0])) {
                $this->_packageInfo[$role] = array($this->_packageInfo[$role]);
            }
            $this->_packageInfo[$role][] =
                array(
                    'name' => $name,
                    'user' => $handle,
                    'email' => $email,
                    'active' => $active,
                );
        } else {
            $testarr = array('lead',
                    'developer', 'contributor', 'helper', 'date', 'time', 'version',
                    'stability', 'license', 'notes', 'contents', 'compatible',
                    'dependencies', 'providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease',
                    'extbinrelease', 'bundle', 'changelog');
            foreach (array('lead', 'developer', 'contributor', 'helper') as $testrole) {
                array_shift($testarr);
                if ($role == $testrole) {
                    break;
                }
            }
            if (!isset($this->_packageInfo[$role])) {
                // ensure that the extends tag is set up in the right location
                $this->_packageInfo = $this->_insertBefore($this->_packageInfo, $testarr,
                    array(), $role);
            }
            $this->_packageInfo[$role] =
                array(
                    'name' => $name,
                    'user' => $handle,
                    'email' => $email,
                    'active' => $active,
                );
        }
        $this->_isValid = 0;
    }

    function updateMaintainer($newrole, $handle, $name, $email, $active = 'yes')
    {
        $found = false;
        foreach (array('lead', 'developer', 'contributor', 'helper') as $role) {
            if (!isset($this->_packageInfo[$role])) {
                continue;
            }
            $info = $this->_packageInfo[$role];
            if (!isset($info[0])) {
                if ($info['user'] == $handle) {
                    $found = true;
                    break;
                }
            }
            foreach ($info as $i => $maintainer) {
                if ($maintainer['user'] == $handle) {
                    $found = $i;
                    break 2;
                }
            }
        }
        if ($found === false) {
            return $this->addMaintainer($newrole, $handle, $name, $email, $active);
        }
        if ($found !== false) {
            if ($found === true) {
                unset($this->_packageInfo[$role]);
            } else {
                unset($this->_packageInfo[$role][$found]);
                $this->_packageInfo[$role] = array_values($this->_packageInfo[$role]);
            }
        }
        $this->addMaintainer($newrole, $handle, $name, $email, $active);
        $this->_isValid = 0;
    }

    function deleteMaintainer($handle)
    {
        $found = false;
        foreach (array('lead', 'developer', 'contributor', 'helper') as $role) {
            if (!isset($this->_packageInfo[$role])) {
                continue;
            }
            if (!isset($this->_packageInfo[$role][0])) {
                $this->_packageInfo[$role] = array($this->_packageInfo[$role]);
            }
            foreach ($this->_packageInfo[$role] as $i => $maintainer) {
                if ($maintainer['user'] == $handle) {
                    $found = $i;
                    break;
                }
            }
            if ($found !== false) {
                unset($this->_packageInfo[$role][$found]);
                if (!count($this->_packageInfo[$role]) && $role == 'lead') {
                    $this->_isValid = 0;
                }
                if (!count($this->_packageInfo[$role])) {
                    unset($this->_packageInfo[$role]);
                    return true;
                }
                $this->_packageInfo[$role] =
                    array_values($this->_packageInfo[$role]);
                if (count($this->_packageInfo[$role]) == 1) {
                    $this->_packageInfo[$role] = $this->_packageInfo[$role][0];
                }
                return true;
            }
            if (count($this->_packageInfo[$role]) == 1) {
                $this->_packageInfo[$role] = $this->_packageInfo[$role][0];
            }
        }
        return false;
    }

    function setReleaseVersion($version)
    {
        if (isset($this->_packageInfo['version']) &&
              isset($this->_packageInfo['version']['release'])) {
            unset($this->_packageInfo['version']['release']);
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $version, array(
            'version' => array('stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'),
            'release' => array('api')));
        $this->_isValid = 0;
    }

    function setAPIVersion($version)
    {
        if (isset($this->_packageInfo['version']) &&
              isset($this->_packageInfo['version']['api'])) {
            unset($this->_packageInfo['version']['api']);
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $version, array(
            'version' => array('stability', 'license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'),
            'api' => array()));
        $this->_isValid = 0;
    }

    /**
     * snapshot|devel|alpha|beta|stable
     */
    function setReleaseStability($state)
    {
        if (isset($this->_packageInfo['stability']) &&
              isset($this->_packageInfo['stability']['release'])) {
            unset($this->_packageInfo['stability']['release']);
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $state, array(
            'stability' => array('license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'),
            'release' => array('api')));
        $this->_isValid = 0;
    }

    /**
     * @param devel|alpha|beta|stable
     */
    function setAPIStability($state)
    {
        if (isset($this->_packageInfo['stability']) &&
              isset($this->_packageInfo['stability']['api'])) {
            unset($this->_packageInfo['stability']['api']);
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $state, array(
            'stability' => array('license', 'notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'),
            'api' => array()));
        $this->_isValid = 0;
    }

    function setLicense($license, $uri = false, $filesource = false)
    {
        if (!isset($this->_packageInfo['license'])) {
            // ensure that the license tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('notes', 'contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), 0, 'license');
        }
        if ($uri || $filesource) {
            $attribs = array();
            if ($uri) {
                $attribs['uri'] = $uri;
            }
            $uri = true; // for test below
            if ($filesource) {
                $attribs['filesource'] = $filesource;
            }
        }
        $license = $uri ? array('attribs' => $attribs, '_content' => $license) : $license;
        $this->_packageInfo['license'] = $license;
        $this->_isValid = 0;
    }

    function setNotes($notes)
    {
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['notes'])) {
            // ensure that the notes tag is set up in the right location
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('contents', 'compatible',
                'dependencies', 'providesextension', 'usesrole', 'usestask', 'srcpackage', 'srcuri',
                'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), $notes, 'notes');
        }
        $this->_packageInfo['notes'] = $notes;
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
     * Reset the listing of package contents
     * @param string base installation dir for the whole package, if any
     */
    function clearContents($baseinstall = false)
    {
        $this->_filesValid = false;
        $this->_isValid = 0;
        if (!isset($this->_packageInfo['contents'])) {
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('compatible',
                    'dependencies', 'providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease',
                    'extbinrelease', 'bundle', 'changelog'), array(), 'contents');
        }
        if ($this->getPackageType() != 'bundle') {
            $this->_packageInfo['contents'] = 
                array('dir' => array('attribs' => array('name' => '/')));
            if ($baseinstall) {
                $this->_packageInfo['contents']['dir']['attribs']['baseinstalldir'] = $baseinstall;
            }
        }
    }

    /**
     * @param string relative path of the bundled package.
     */
    function addBundledPackage($path)
    {
        if ($this->getPackageType() != 'bundle') {
            return false;
        }
        $this->_filesValid = false;
        $this->_isValid = 0;
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $path, array(
                'contents' => array('compatible', 'dependencies', 'providesextension',
                'usesrole', 'usestask', 'srcpackage', 'srcuri', 'phprelease',
                'extsrcrelease', 'extbinrelease', 'bundle', 'changelog'),
                'bundledpackage' => array()));
    }

    /**
     * @param string file name
     * @param PEAR_Task_Common a read/write task
     */
    function addTaskToFile($filename, $task)
    {
        if (!method_exists($task, 'getXml')) {
            return false;
        }
        if (!method_exists($task, 'getName')) {
            return false;
        }
        if (!method_exists($task, 'validate')) {
            return false;
        }
        if (!$task->validate()) {
            return false;
        }
        if (!isset($this->_packageInfo['contents']['dir']['file'])) {
            return false;
        }
        $this->getTasksNs(); // discover the tasks namespace if not done already
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
                        $t = isset($this->_packageInfo['contents']['dir']['file'][$i]
                              ['attribs'][$this->_tasksNs .
                              ':' . $task->getName()]) ?
                              $this->_packageInfo['contents']['dir']['file'][$i]
                              ['attribs'][$this->_tasksNs .
                              ':' . $task->getName()] : false;
                        if ($t && !isset($t[0])) {
                            $this->_packageInfo['contents']['dir']['file'][$i]
                                [$this->_tasksNs . ':' . $task->getName()] = array($t);
                        }
                        $this->_packageInfo['contents']['dir']['file'][$i][$this->_tasksNs .
                            ':' . $task->getName()][] = $task->getXml();
                    } else {
                        $t = isset($this->_packageInfo['contents']['dir']['file']
                              ['attribs'][$this->_tasksNs .
                              ':' . $task->getName()]) ? $this->_packageInfo['contents']['dir']['file']
                              ['attribs'][$this->_tasksNs .
                              ':' . $task->getName()] : false;
                        if ($t && !isset($t[0])) {
                            $this->_packageInfo['contents']['dir']['file']
                                [$this->_tasksNs . ':' . $task->getName()] = array($t);
                        }
                        $this->_packageInfo['contents']['dir']['file'][$this->_tasksNs .
                            ':' . $task->getName()][] = $task->getXml();
                    }
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @param string path to the file
     * @param string filename
     * @param array extra attributes
     */
    function addFile($dir, $file, $attrs)
    {
        if ($this->getPackageType() == 'bundle') {
            return false;
        }
        $this->_filesValid = false;
        $this->_isValid = 0;
        $dir = preg_replace(array('!\\\\+!', '!/+!'), array('/', '/'), $dir);
        if ($dir == '/' || $dir == '') {
            $dir = '';
        } else {
            $dir .= '/';
        }
        $attrs['name'] = $dir . $file;
        if (!isset($this->_packageInfo['contents'])) {
            // ensure that the contents tag is set up
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo,
                array('compatible', 'dependencies', 'providesextension', 'usesrole', 'usestask',
                'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease',
                'extbinrelease', 'bundle', 'changelog'), array(), 'contents');
        }
        if (isset($this->_packageInfo['contents']['dir']['file'])) {
            if (!isset($this->_packageInfo['contents']['dir']['file'][0])) {
                $this->_packageInfo['contents']['dir']['file'] =
                    array($this->_packageInfo['contents']['dir']['file']);
            }
            $this->_packageInfo['contents']['dir']['file'][]['attribs'] = $attrs;
        } else {
            $this->_packageInfo['contents']['dir']['file']['attribs'] = $attrs;
        }
    }

    /**
     * @param string Dependent package name
     * @param string Dependent package's channel name
     * @param string minimum version of specified package that this release is guaranteed to be
     *               compatible with
     * @param string maximum version of specified package that this release is guaranteed to be
     *               compatible with
     * @param string versions of specified package that this release is not compatible with
     */
    function addCompatiblePackage($name, $channel, $min, $max, $exclude = false)
    {
        $this->_isValid = 0;
        $set = array(
            'name' => $name,
            'channel' => $channel,
            'min' => $min,
            'max' => $max,
        );
        if ($exclude) {
            $set['exclude'] = $exclude;
        }
        $this->_isValid = 0;
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $set, array(
                'compatible' => array('dependencies', 'providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog')
            ));
    }

    /**
     * Removes the <usesrole> tag entirely
     */
    function resetUsesrole()
    {
        if (isset($this->_packageInfo['usesrole'])) {
            unset($this->_packageInfo['usesrole']);
        }
    }

    /**
     * @param string
     * @param string package name or uri
     * @param string channel name if non-uri
     */
    function addUsesrole($role, $packageOrUri, $channel = false) {
        $set = array('role' => $role);
        if ($channel) {
            $set['package'] = $packageOrUri;
            $set['channel'] = $channel;
        } else {
            $set['uri'] = $packageOrUri;
        }
        $this->_isValid = 0;
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $set, array(
                'usesrole' => array('usestask', 'srcpackage', 'srcuri',
                    'phprelease', 'extsrcrelease', 'extbinrelease', 'bundle', 'changelog')
            ));
    }

    /**
     * Removes the <usestask> tag entirely
     */
    function resetUsestask()
    {
        if (isset($this->_packageInfo['usestask'])) {
            unset($this->_packageInfo['usestask']);
        }
    }


    /**
     * @param string
     * @param string package name or uri
     * @param string channel name if non-uri
     */
    function addUsestask($task, $packageOrUri, $channel = false) {
        $set = array('task' => $task);
        if ($channel) {
            $set['package'] = $packageOrUri;
            $set['channel'] = $channel;
        } else {
            $set['uri'] = $packageOrUri;
        }
        $this->_isValid = 0;
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $set, array(
                'usestask' => array('srcpackage', 'srcuri',
                    'phprelease', 'extsrcrelease', 'extbinrelease', 'bundle', 'changelog')
            ));
    }

    /**
     * Remove all compatible tags
     */
    function clearCompatible()
    {
        unset($this->_packageInfo['compatible']);
    }

    /**
     * Reset dependencies prior to adding new ones
     */
    function clearDeps()
    {
        if (!isset($this->_packageInfo['dependencies'])) {
            $this->_packageInfo = $this->_mergeTag($this->_packageInfo, array(),
                array(
                    'dependencies' => array('providesextension', 'usesrole', 'usestask',
                        'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                        'bundle', 'changelog')));
        }
        $this->_packageInfo['dependencies'] = array();
    }

    /**
     * @param string minimum PHP version allowed
     * @param string maximum PHP version allowed
     * @param array $exclude incompatible PHP versions
     */
    function setPhpDep($min, $max = false, $exclude = false)
    {
        $this->_isValid = 0;
        $dep =
            array(
                'min' => $min,
            );
        if ($max) {
            $dep['max'] = $max;
        }
        if ($exclude) {
            if (count($exclude) == 1) {
                $exclude = $exclude[0];
            }
            $dep['exclude'] = $exclude;
        }
        if (isset($this->_packageInfo['dependencies']['required']['php'])) {
            $this->_stack->push(__FUNCTION__, 'warning', array('dep' =>
            $this->_packageInfo['dependencies']['required']['php']),
                'warning: PHP dependency already exists, overwriting');
            unset($this->_packageInfo['dependencies']['required']['php']);
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'required' => array('optional', 'group'),
                'php' => array('pearinstaller', 'package', 'subpackage', 'extension', 'os', 'arch')
            ));
        return true;
    }

    /**
     * @param string minimum allowed PEAR installer version
     * @param string maximum allowed PEAR installer version
     * @param string recommended PEAR installer version
     * @param array incompatible version of the PEAR installer
     */
    function setPearinstallerDep($min, $max = false, $recommended = false, $exclude = false)
    {
        $this->_isValid = 0;
        $dep =
            array(
                'min' => $min,
            );
        if ($max) {
            $dep['max'] = $max;
        }
        if ($recommended) {
            $dep['recommended'] = $recommended;
        }
        if ($exclude) {
            if (count($exclude) == 1) {
                $exclude = $exclude[0];
            }
            $dep['exclude'] = $exclude;
        }
        if (isset($this->_packageInfo['dependencies']['required']['pearinstaller'])) {
            $this->_stack->push(__FUNCTION__, 'warning', array('dep' =>
            $this->_packageInfo['dependencies']['required']['pearinstaller']),
                'warning: PEAR Installer dependency already exists, overwriting');
            unset($this->_packageInfo['dependencies']['required']['pearinstaller']);
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'required' => array('optional', 'group'),
                'pearinstaller' => array('package', 'subpackage', 'extension', 'os', 'arch')
            ));
    }

    /**
     * Mark a package as conflicting with this package
     * @param string package name
     * @param string package channel
     * @param string extension this package provides, if any
     */
    function addConflictingPackageDepWithChannel($name, $channel, $providesextension = false)
    {
        $this->_isValid = 0;
        $dep =
            array(
                'name' => $name,
                'channel' => $channel,
                'conflicts' => '',
            );
        if ($providesextension) {
            $dep['providesextension'] = $providesextension;
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'required' => array('optional', 'group'),
                'package' => array('subpackage', 'extension', 'os', 'arch')
            ));
    }

    /**
     * Mark a package as conflicting with this package
     * @param string package name
     * @param string package channel
     * @param string extension this package provides, if any
     */
    function addConflictingPackageDepWithUri($name, $uri, $providesextension = false)
    {
        $this->_isValid = 0;
        $dep =
            array(
                'name' => $name,
                'uri' => $uri,
                'conflicts' => '',
            );
        if ($providesextension) {
            $dep['providesextension'] = $providesextension;
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'required' => array('optional', 'group'),
                'package' => array('subpackage', 'extension', 'os', 'arch')
            ));
    }

    function addDependencyGroup($name, $hint)
    {
        $this->_isValid = 0;
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo,
            array('attribs' => array('name' => $name, 'hint' => $hint)),
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'group' => array(),
            ));
    }

    /**
     * @param string package name
     * @param string|false channel name, false if this is a uri
     * @param string|false uri name, false if this is a channel
     * @param string|false minimum version required
     * @param string|false maximum version allowed
     * @param string|false recommended installation version
     * @param array|false versions to exclude from installation
     * @param string extension this package provides, if any
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     * @return array
     * @access private
     */
    function _constructDep($name, $channel, $uri, $min, $max, $recommended, $exclude,
                           $providesextension = false, $nodefault = false)
    {
        $dep =
            array(
                'name' => $name,
            );
        if ($channel) {
            $dep['channel'] = $channel;
        } elseif ($uri) {
            $dep['uri'] = $uri;
        }
        if ($min) {
            $dep['min'] = $min;
        }
        if ($max) {
            $dep['max'] = $max;
        }
        if ($recommended) {
            $dep['recommended'] = $recommended;
        }
        if ($exclude) {
            if (is_array($exclude) && count($exclude) == 1) {
                $exclude = $exclude[0];
            }
            $dep['exclude'] = $exclude;
        }
        if ($nodefault) {
            $dep['nodefault'] = '';
        }
        if ($providesextension) {
            $dep['providesextension'] = $providesextension;
        }
        return $dep;
    }

    /**
     * @param package|subpackage
     * @param string group name
     * @param string package name
     * @param string package channel
     * @param string minimum version
     * @param string maximum version
     * @param string recommended version
     * @param array|false optional excluded versions
     * @param string extension this package provides, if any
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     * @return bool false if the dependency group has not been initialized with
     *              {@link addDependencyGroup()}, or a subpackage is added with
     *              a providesextension
     */
    function addGroupPackageDepWithChannel($type, $groupname, $name, $channel, $min = false,
                                      $max = false, $recommended = false, $exclude = false,
                                      $providesextension = false, $nodefault = false)
    {
        if ($type == 'subpackage' && $providesextension) {
            return false; // subpackages must be php packages
        }
        $dep = $this->_constructDep($name, $channel, false, $min, $max, $recommended, $exclude,
            $providesextension, $nodefault);
        return $this->_addGroupDependency($type, $dep, $groupname);
    }

    /**
     * @param package|subpackage
     * @param string group name
     * @param string package name
     * @param string package uri
     * @param string extension this package provides, if any
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     * @return bool false if the dependency group has not been initialized with
     *              {@link addDependencyGroup()}
     */
    function addGroupPackageDepWithURI($type, $groupname, $name, $uri, $providesextension = false,
                                       $nodefault = false)
    {
        if ($type == 'subpackage' && $providesextension) {
            return false; // subpackages must be php packages
        }
        $dep = $this->_constructDep($name, false, $uri, false, false, false, false,
            $providesextension, $nodefault);
        return $this->_addGroupDependency($type, $dep, $groupname);
    }

    /**
     * @param string group name (must be pre-existing)
     * @param string extension name
     * @param string minimum version allowed
     * @param string maximum version allowed
     * @param string recommended version
     * @param array incompatible versions
     */
    function addGroupExtensionDep($groupname, $name, $min = false, $max = false,
                                         $recommended = false, $exclude = false)
    {
        $this->_isValid = 0;
        $dep = $this->_constructDep($name, false, false, $min, $max, $recommended, $exclude);
        return $this->_addGroupDependency('extension', $dep, $groupname);
    }

    /**
     * @param package|subpackage|extension
     * @param array dependency contents
     * @param string name of the dependency group to add this to
     * @return boolean
     * @access private
     */
    function _addGroupDependency($type, $dep, $groupname)
    {
        $arr = array('subpackage', 'extension');
        if ($type != 'package') {
            array_shift($arr);
        }
        if ($type == 'extension') {
            array_shift($arr);
        }
        if (!isset($this->_packageInfo['dependencies']['group'])) {
            return false;
        } else {
            if (!isset($this->_packageInfo['dependencies']['group'][0])) {
                if ($this->_packageInfo['dependencies']['group']['attribs']['name'] == $groupname) {
                    $this->_packageInfo['dependencies']['group'] = $this->_mergeTag(
                        $this->_packageInfo['dependencies']['group'], $dep,
                        array(
                            $type => $arr
                        ));
                    $this->_isValid = 0;
                    return true;
                } else {
                    return false;
                }
            } else {
                foreach ($this->_packageInfo['dependencies']['group'] as $i => $group) {
                    if ($group['attribs']['name'] == $groupname) {
                    $this->_packageInfo['dependencies']['group'][$i] = $this->_mergeTag(
                        $this->_packageInfo['dependencies']['group'][$i], $dep,
                        array(
                            $type => $arr
                        ));
                        $this->_isValid = 0;
                        return true;
                    }
                }
                return false;
            }
        }
    }

    /**
     * @param optional|required
     * @param string package name
     * @param string package channel
     * @param string minimum version
     * @param string maximum version
     * @param string recommended version
     * @param string extension this package provides, if any
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     * @param array|false optional excluded versions
     */
    function addPackageDepWithChannel($type, $name, $channel, $min = false, $max = false,
                                      $recommended = false, $exclude = false,
                                      $providesextension = false, $nodefault = false)
    {
        $this->_isValid = 0;
        $arr = array('optional', 'group');
        if ($type != 'required') {
            array_shift($arr);
        }
        $dep = $this->_constructDep($name, $channel, false, $min, $max, $recommended, $exclude,
            $providesextension, $nodefault);
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                $type => $arr,
                'package' => array('subpackage', 'extension', 'os', 'arch')
            ));
    }

    /**
     * @param optional|required
     * @param string name of the package
     * @param string uri of the package
     * @param string extension this package provides, if any
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     */
    function addPackageDepWithUri($type, $name, $uri, $providesextension = false,
                                  $nodefault = false)
    {
        $this->_isValid = 0;
        $arr = array('optional', 'group');
        if ($type != 'required') {
            array_shift($arr);
        }
        $dep = $this->_constructDep($name, false, $uri, false, false, false, false,
            $providesextension, $nodefault);
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                $type => $arr,
                'package' => array('subpackage', 'extension', 'os', 'arch')
            ));
    }

    /**
     * @param optional|required optional, required
     * @param string package name
     * @param string package channel
     * @param string minimum version
     * @param string maximum version
     * @param string recommended version
     * @param array incompatible versions
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     */
    function addSubpackageDepWithChannel($type, $name, $channel, $min = false, $max = false,
                                         $recommended = false, $exclude = false,
                                         $nodefault = false)
    {
        $this->_isValid = 0;
        $arr = array('optional', 'group');
        if ($type != 'required') {
            array_shift($arr);
        }
        $dep = $this->_constructDep($name, $channel, false, $min, $max, $recommended, $exclude,
            $nodefault);
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                $type => $arr,
                'subpackage' => array('extension', 'os', 'arch')
            ));
    }

    /**
     * @param optional|required optional, required
     * @param string package name
     * @param string package uri for download
     * @param bool if true, tells the installer to ignore the default optional dependency group
     *             when installing this package
     */
    function addSubpackageDepWithUri($type, $name, $uri, $nodefault = false)
    {
        $this->_isValid = 0;
        $arr = array('optional', 'group');
        if ($type != 'required') {
            array_shift($arr);
        }
        $dep = $this->_constructDep($name, false, $uri, false, false, false, false, $nodefault);
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                $type => $arr,
                'subpackage' => array('extension', 'os', 'arch')
            ));
    }

    /**
     * @param optional|required optional, required
     * @param string extension name
     * @param string minimum version
     * @param string maximum version
     * @param string recommended version
     * @param array incompatible versions
     */
    function addExtensionDep($type, $name, $min = false, $max = false, $recommended = false,
                             $exclude = false)
    {
        $this->_isValid = 0;
        $arr = array('optional', 'group');
        if ($type != 'required') {
            array_shift($arr);
        }
        $dep = $this->_constructDep($name, false, false, $min, $max, $recommended, $exclude);
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                $type => $arr,
                'extension' => array('os', 'arch')
            ));
    }

    /**
     * @param string Operating system name
     * @param boolean true if this package cannot be installed on this OS
     */
    function addOsDep($name, $conflicts = false)
    {
        $this->_isValid = 0;
        $dep = array('name' => $name);
        if ($conflicts) {
            $dep['conflicts'] = '';
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'required' => array('optional', 'group'),
                'os' => array('arch')
            ));
    }

    /**
     * @param string Architecture matching pattern
     * @param boolean true if this package cannot be installed on this architecture
     */
    function addArchDep($pattern, $conflicts = false)
    {
        $this->_isValid = 0;
        $dep = array('pattern' => $pattern);
        if ($conflicts) {
            $dep['conflicts'] = '';
        }
        $this->_packageInfo = $this->_mergeTag($this->_packageInfo, $dep,
            array(
                'dependencies' => array('providesextension', 'usesrole', 'usestask',
                    'srcpackage', 'srcuri', 'phprelease', 'extsrcrelease', 'extbinrelease',
                    'bundle', 'changelog'),
                'required' => array('optional', 'group'),
                'arch' => array()
            ));
    }

    /**
     * Set the kind of package, and erase all release tags
     *
     * - a php package is a PEAR-style package
     * - an extbin package is a PECL-style extension binary
     * - an extsrc package is a PECL-style source for a binary
     * - a bundle package is a collection of other pre-packaged packages
     * @param php|extbin|extsrc|bundle
     * @return bool success
     */
    function setPackageType($type)
    {
        $this->_isValid = 0;
        if (!in_array($type, array('php', 'extbin', 'extsrc', 'bundle'))) {
            return false;
        }
        if ($type != 'bundle') {
            $type .= 'release';
        }
        foreach (array('phprelease', 'extbinrelease', 'extsrcrelease', 'bundle') as $test) {
            unset($this->_packageInfo[$test]);
        }
        if (!isset($this->_packageInfo[$type])) {
            // ensure that the release tag is set up
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo, array('changelog'),
                array(), $type);
        }
        $this->_packageInfo[$type] = array();
        return true;
    }

    /**
     * @return bool true if package type is set up
     */
    function addRelease()
    {
        if ($type = $this->getPackageType()) {
            if ($type != 'bundle') {
                $type .= 'release';
            }
            $this->_packageInfo = $this->_mergeTag($this->_packageInfo, array(),
                array($type => array('changelog')));
            return true;
        }
        return false;
    }

    /**
     * Get the current release tag in order to add to it
     * @param bool returns only releases that have installcondition if true
     * @return array|null
     */
    function &_getCurrentRelease($strict = true)
    {
        if ($p = $this->getPackageType()) {
            if ($strict) {
                if ($p == 'extsrc') {
                    $a = null;
                    return $a;
                }
            }
            if ($p != 'bundle') {
                $p .= 'release';
            }
            if (isset($this->_packageInfo[$p][0])) {
                return $this->_packageInfo[$p][count($this->_packageInfo[$p]) - 1];
            } else {
                return $this->_packageInfo[$p];
            }
        } else {
            $a = null;
            return $a;
        }
    }

    /**
     * Add a file to the current release that should be installed under a different name
     * @param string <contents> path to file
     * @param string name the file should be installed as
     */
    function addInstallAs($path, $as)
    {
        $r = &$this->_getCurrentRelease();
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        $r = $this->_mergeTag($r, array('attribs' => array('name' => $path, 'as' => $as)),
            array(
                'filelist' => array(),
                'install' => array('ignore')
            ));
    }

    /**
     * Add a file to the current release that should be ignored
     * @param string <contents> path to file
     * @return bool success of operation
     */
    function addIgnore($path)
    {
        $r = &$this->_getCurrentRelease();
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        $r = $this->_mergeTag($r, array('attribs' => array('name' => $path)),
            array(
                'filelist' => array(),
                'ignore' => array()
            ));
    }

    /**
     * Add an extension binary package for this extension source code release
     *
     * Note that the package must be from the same channel as the extension source package
     * @param string
     */
    function addBinarypackage($package)
    {
        if ($this->getPackageType() != 'extsrc') {
            return false;
        }
        $r = &$this->_getCurrentRelease(false);
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        $r = $this->_mergeTag($r, $package,
            array(
                'binarypackage' => array('filelist'),
            ));
    }

    /**
     * Add a configureoption to an extension source package
     * @param string
     * @param string
     * @param string
     */
    function addConfigureOption($name, $prompt, $default = null)
    {
        if ($this->getPackageType() != 'extsrc') {
            return false;
        }
        $r = &$this->_getCurrentRelease(false);
        if ($r === null) {
            return false;
        }
        $opt = array('attribs' => array('name' => $name, 'prompt' => $prompt));
        if ($default !== null) {
            $opt['default'] = $default;
        }
        $this->_isValid = 0;
        $r = $this->_mergeTag($r, $opt,
            array(
                'configureoption' => array('binarypackage', 'filelist'),
            ));
    }

    /**
     * Set an installation condition based on php version for the current release set
     * @param string minimum version
     * @param string maximum version
     * @param false|array incompatible versions of PHP
     */
    function setPhpInstallCondition($min, $max, $exclude = false)
    {
        $r = &$this->_getCurrentRelease();
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        if (isset($r['installconditions']['php'])) {
            unset($r['installconditions']['php']);
        }
        $dep = array('min' => $min, 'max' => $max);
        if ($exclude) {
            if (is_array($exclude) && count($exclude) == 1) {
                $exclude = $exclude[0];
            }
            $dep['exclude'] = $exclude;
        }
        if ($this->getPackageType() == 'extsrc') {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('configureoption', 'binarypackage',
                        'filelist'),
                    'php' => array('extension', 'os', 'arch')
                ));
        } else {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('filelist'),
                    'php' => array('extension', 'os', 'arch')
                ));
        }
    }

    /**
     * @param optional|required optional, required
     * @param string extension name
     * @param string minimum version
     * @param string maximum version
     * @param string recommended version
     * @param array incompatible versions
     */
    function addExtensionInstallCondition($name, $min = false, $max = false, $recommended = false,
                                          $exclude = false)
    {
        $r = &$this->_getCurrentRelease();
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        $dep = $this->_constructDep($name, false, false, $min, $max, $recommended, $exclude);
        if ($this->getPackageType() == 'extsrc') {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('configureoption', 'binarypackage',
                        'filelist'),
                    'extension' => array('os', 'arch')
                ));
        } else {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('filelist'),
                    'extension' => array('os', 'arch')
                ));
        }
    }

    /**
     * Set an installation condition based on operating system for the current release set
     * @param string OS name
     * @param bool whether this OS is incompatible with the current release
     */
    function setOsInstallCondition($name, $conflicts = false)
    {
        $r = &$this->_getCurrentRelease();
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        if (isset($r['installconditions']['os'])) {
            unset($r['installconditions']['os']);
        }
        $dep = array('name' => $name);
        if ($conflicts) {
            $dep['conflicts'] = '';
        }
        if ($this->getPackageType() == 'extsrc') {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('configureoption', 'binarypackage',
                        'filelist'),
                    'os' => array('arch')
                ));
        } else {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('filelist'),
                    'os' => array('arch')
                ));
        }
    }

    /**
     * Set an installation condition based on architecture for the current release set
     * @param string architecture pattern
     * @param bool whether this arch is incompatible with the current release
     */
    function setArchInstallCondition($pattern, $conflicts = false)
    {
        $r = &$this->_getCurrentRelease();
        if ($r === null) {
            return false;
        }
        $this->_isValid = 0;
        if (isset($r['installconditions']['arch'])) {
            unset($r['installconditions']['arch']);
        }
        $dep = array('pattern' => $pattern);
        if ($conflicts) {
            $dep['conflicts'] = '';
        }
        if ($this->getPackageType() == 'extsrc') {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('configureoption', 'binarypackage',
                        'filelist'),
                    'arch' => array()
                ));
        } else {
            $r = $this->_mergeTag($r, $dep,
                array(
                    'installconditions' => array('filelist'),
                    'arch' => array()
                ));
        }
    }

    /**
     * For extension binary releases, this is used to specify either the
     * static URI to a source package, or the package name and channel of the extsrc
     * package it is based on.
     * @param string Package name, or full URI to source package (extsrc type)
     */
    function setSourcePackage($packageOrUri)
    {
        $this->_isValid = 0;
        if (isset($this->_packageInfo['channel'])) {
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo, array('phprelease',
                'extsrcrelease', 'extbinrelease', 'bundle', 'changelog'),
                $packageOrUri, 'srcpackage');
        } else {
            $this->_packageInfo = $this->_insertBefore($this->_packageInfo, array('phprelease',
                'extsrcrelease', 'extbinrelease', 'bundle', 'changelog'), $packageOrUri, 'srcuri');
        }
    }

    /**
     * Generate a valid change log entry from the current package.xml
     * @param string|false
     */
    function generateChangeLogEntry($notes = false)
    {
        return array(
            'version' => 
                array(
                    'release' => $this->getVersion('release'),
                    'api' => $this->getVersion('api'),
                    ),
            'stability' =>
                $this->getStability(),
            'date' => $this->getDate(),
            'license' => $this->getLicense(true),
            'notes' => $notes ? $notes : $this->getNotes()
            );
    }

    /**
     * @param string release version to set change log notes for
     * @param array output of {@link generateChangeLogEntry()}
     */
    function setChangelogEntry($releaseversion, $contents)
    {
        if (!isset($this->_packageInfo['changelog'])) {
            $this->_packageInfo['changelog']['release'] = $contents;
            return;
        }
        if (!isset($this->_packageInfo['changelog']['release'][0])) {
            if ($this->_packageInfo['changelog']['release']['version']['release'] == $releaseversion) {
                $this->_packageInfo['changelog']['release'] = array(
                    $this->_packageInfo['changelog']['release']);
            } else {
                $this->_packageInfo['changelog']['release'] = array(
                    $this->_packageInfo['changelog']['release']);
                return $this->_packageInfo['changelog']['release'][] = $contents;
            }
        }
        foreach($this->_packageInfo['changelog']['release'] as $index => $changelog) {
            if (isset($changelog['version']) &&
                  strnatcasecmp($changelog['version']['release'], $releaseversion) == 0) {
                $curlog = $index;
            }
        }
        if (isset($curlog)) {
            $this->_packageInfo['changelog']['release'][$curlog] = $contents;
        } else {
            $this->_packageInfo['changelog']['release'][] = $contents;
        }
    }

    /**
     * Remove the changelog entirely
     */
    function clearChangeLog()
    {
        unset($this->_packageInfo['changelog']);
    }
}
?>