<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';
require_once 'Archive/Tar.php';
require_once 'System.php';

/**
 * List of temporary files and directories registered by
 * PEAR_Common::addTempFile().
 * @var array
 */
$GLOBALS['_PEAR_Common_tempfiles'] = array();

/*
 * TODO:
 *   - check in inforFromDescFile that the minimal data needed is present
 *     (pack name, version, files, others?)
 *   - inherance of dir attribs to files may fail under certain circumstances
 */

/**
 * Class providing common functionality for PEAR adminsitration classes.
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
     * Valid maintainer roles
     * @var array
     */
    var $maintainer_roles = array('lead','developer','contributor','helper');

    /**
     * Valid release states
     * @var array
     */
    var $release_states  = array('alpha','beta','stable','snapshot','devel');

    /**
     * Valid dependency types
     * @var array
     */
    var $dependency_types  = array('pkg','ext','php','prog','ldlib','rtlib','os','websrv','sapi');

    /**
     * Valid dependency relations
     * @var array
     */
    var $dependency_relations  = array('has','eq','lt','le','gt','ge');

    /**
     * Valid file roles
     * @var array
     */
    var $file_roles  = array('php','ext','test','doc','data','extsrc');

    /**
     * User Interface object (PEAR_Frontend_* class).  If null,
     * log() uses print.
     * @var object
     */
    var $ui = null;

    // }}}

    // {{{ constructor

    /**
     * PEAR_Common constructor
     *
     * @access public
     */
    function PEAR_Common()
    {
        $this->PEAR();
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
                System::rm("-rf $file");
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
     * @param string   name of file or directory
     *
     * @return void
     *
     * @access public
     */
    function addTempFile($file)
    {
        $GLOBALS['_PEAR_Common_tempfiles'][] = $file;
    }

    // }}}
    // {{{ mkDirHier()

    /**
     * Wrapper to System::mkDir(), creates a directory as well as
     * any necessary parent directories.
     *
     * @param string  directory name
     *
     * @return bool TRUE on success, or a PEAR error
     *
     * @access public
     */
    function mkDirHier($dir)
    {
        $this->log(2, "+ create dir $dir");
        return System::mkDir("-p $dir");
    }

    // }}}
    // {{{ log()

    /**
     * Logging method.
     *
     * @param int    log level (0 is quiet, higher is noisier)
     *
     * @param string message to write to the log
     *
     * @return void
     *
     * @access public
     */
    function log($level, $msg)
    {
        if ($this->debug >= $level) {
            if (is_object($this->ui)) {
                $this->ui->displayLine($msg);
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
     * @param string (optional) Directory to use as tmpdir.  Will use
     * system defaults (for example /tmp or c:\windows\temp) if not
     * specified
     *
     * @return string name of created directory
     *
     * @access public
     */
    function mkTempDir($tmpdir = '')
    {
        if ($tmpdir) {
            $topt = "-t $tmpdir ";
        } else {
            $topt = '';
        }
        if (!$tmpdir = System::mktemp($topt . '-d pear')) {
            return false;
        }
        $this->addTempFile($tmpdir);
        return $tmpdir;
    }

    // }}}
    // {{{ setFrontend()

    function setFrontend(&$ui)
    {
        $this->ui = &$ui;
    }

    // }}}

    // {{{ _element_start()

    /**
     * XML parser callback for starting elements.  Used while package
     * format version is not yet known.
     *
     * @param resource  XML parser resource
     * @param string    name of starting element
     * @param array     element attributes, name => value
     *
     * @return void
     *
     * @access private
     */
    function _element_start($xp, $name, $attribs)
    {
        array_push($this->element_stack, $name);
        $this->current_element = $name;
        $spos = sizeof($this->element_stack) - 2;
        $this->prev_element = ($spos >= 0) ? $this->element_stack[$spos] : '';
        $this->current_attributes = $attribs;
        switch ($name) {
            case 'package': {
                if (isset($attribs['version'])) {
                    $vs = preg_replace('/[^0-9a-z]/', '_', $attribs['version']);
                } else {
                    $vs = '1_0';
                }
                $elem_start = '_element_start_'. $vs;
                $elem_end = '_element_end_'. $vs;
                $cdata = '_pkginfo_cdata_'. $vs;
                xml_set_element_handler($xp, $elem_start, $elem_end);
                xml_set_character_data_handler($xp, $cdata);
                break;
            }
        }
    }

    // }}}
    // {{{ _element_end()

    /**
     * XML parser callback for ending elements.  Used while package
     * format version is not yet known.
     *
     * @param resource  XML parser resource
     * @param string    name of ending element
     *
     * @return void
     *
     * @access private
     */
    function _element_end($xp, $name)
    {
    }

    // }}}

    // Support for package DTD v1.0:
    // {{{ _element_start_1_0()

    /**
     * XML parser callback for ending elements.  Used for version 1.0
     * packages.
     *
     * @param resource  XML parser resource
     * @param string    name of ending element
     *
     * @return void
     *
     * @access private
     */
    function _element_start_1_0($xp, $name, $attribs)
    {
        array_push($this->element_stack, $name);
        $this->current_element = $name;
        $spos = sizeof($this->element_stack) - 2;
        $this->prev_element = ($spos >= 0) ? $this->element_stack[$spos] : '';
        $this->current_attributes = $attribs;
        $this->cdata = '';
        switch ($name) {
            case 'dir':
                if ($attribs['name'] != '/') {
                    $this->dir_names[] = $attribs['name'];
                }
                if (isset($attribs['baseinstalldir'])) {
                    $this->dir_install = $attribs['baseinstalldir'];
                }
                if (isset($attribs['role'])) {
                    $this->dir_role = $attribs['role'];
                }
                break;
            case 'libfile':
                $this->lib_atts = $attribs;
                $this->lib_atts['role'] = 'extsrc';
                break;
            case 'maintainers':
                $this->pkginfo['maintainers'] = array();
                $this->m_i = 0; // maintainers array index
                break;
            case 'maintainer':
                // compatibility check
                if (!isset($this->pkginfo['maintainers'])) {
                    $this->pkginfo['maintainers'] = array();
                    $this->m_i = 0;
                }
                $this->pkginfo['maintainers'][$this->m_i] = array();
                $this->current_maintainer =& $this->pkginfo['maintainers'][$this->m_i];
                break;
            case 'changelog':
                $this->pkginfo['changelog'] = array();
                $this->c_i = 0; // changelog array index
                $this->in_changelog = true;
                break;
            case 'release':
                if ($this->in_changelog) {
                    $this->pkginfo['changelog'][$this->c_i] = array();
                    $this->current_release =& $this->pkginfo['changelog'][$this->c_i];
                }
                break;
            case 'deps':
                if (!$this->in_changelog) {
                    $this->pkginfo['release_deps'] = array();
                }
                break;
            case 'dep':
                // dependencies array index
                if (!$this->in_changelog) {
                    $this->d_i = (isset($this->d_i)) ? $this->d_i + 1 : 0;
                    $this->pkginfo['release_deps'][$this->d_i] = $attribs;
                }
                break;
        }
    }

    // }}}
    // {{{ _element_end_1_0()

    /**
     * XML parser callback for ending elements.  Used for version 1.0
     * packages.
     *
     * @param resource  XML parser resource
     * @param string    name of ending element
     *
     * @return void
     *
     * @access private
     */
    function _element_end_1_0($xp, $name)
    {
        $data = trim($this->cdata);
        switch ($name) {
            case 'name':
                switch ($this->prev_element) {
                    case 'package':
                        $this->pkginfo['package'] = ereg_replace('[^a-zA-Z0-9._]', '_', $data);
                        break;
                    case 'maintainer':
                        $this->current_maintainer['name'] = $data;
                        break;
                }
                break;
            case 'summary':
                $this->pkginfo['summary'] = $data;
                break;
            case 'description':
                $this->pkginfo['description'] = $data;
                break;
            case 'user':
                $this->current_maintainer['handle'] = $data;
                break;
            case 'email':
                $this->current_maintainer['email'] = $data;
                break;
            case 'role':
                $this->current_maintainer['role'] = $data;
                break;
            case 'version':
                $data = ereg_replace ('[^a-zA-Z0-9._\-]', '_', $data);
                if ($this->in_changelog) {
                    $this->current_release['version'] = $data;
                } else {
                    $this->pkginfo['version'] = $data;
                }
                break;
            case 'date':
                if ($this->in_changelog) {
                    $this->current_release['release_date'] = $data;
                } else {
                    $this->pkginfo['release_date'] = $data;
                }
                break;
            case 'notes':
                if ($this->in_changelog) {
                    $this->current_release['release_notes'] = $data;
                } else {
                    $this->pkginfo['release_notes'] = $data;
                }
                break;
            case 'state':
                if ($this->in_changelog) {
                    $this->current_release['release_state'] = $data;
                } else {
                    $this->pkginfo['release_state'] = $data;
                }
                break;
            case 'license':
                $this->pkginfo['release_license'] = $data;
                break;
            case 'sources':
                $this->lib_sources[] = $data;
                break;
            case 'dep':
                if ($data = trim($data)) {
                    $this->pkginfo['release_deps'][$this->d_i]['name'] = $data;
                }
                break;
            case 'dir':
                array_pop($this->dir_names);
                break;
            case 'file':
                $this->current_file = $data;
                $path = '';
                if (count($this->dir_names)) {
                    foreach ($this->dir_names as $dir) {
                        $path .= $dir . DIRECTORY_SEPARATOR;
                    }
                }
                $path .= $this->current_file;
                $this->filelist[$path] = $this->current_attributes;
                // Set the baseinstalldir only if the file don't have this attrib
                if (!isset($this->filelist[$path]['baseinstalldir']) &&
                    isset($this->dir_install))
                {
                    $this->filelist[$path]['baseinstalldir'] = $this->dir_install;
                }
                // Set the Role
                if (!isset($this->filelist[$path]['role']) && isset($this->dir_role)) {
                    $this->filelist[$path]['role'] = $this->dir_role;
                }
                break;
            case 'libfile':
                $path = '';
                if (!empty($this->dir_names)) {
                    foreach ($this->dir_names as $dir) {
                        $path .= $dir . DIRECTORY_SEPARATOR;
                    }
                }
                $path .= $this->lib_name;
                $this->filelist[$path] = $this->lib_atts;
                // Set the baseinstalldir only if the file don't have this attrib
                if (!isset($this->filelist[$path]['baseinstalldir']) &&
                    isset($this->dir_install))
                {
                    $this->filelist[$path]['baseinstalldir'] = $this->dir_install;
                }
                if (isset($this->lib_sources)) {
                    $this->filelist[$path]['sources'] = implode(' ', $this->lib_sources);
                }
                unset($this->lib_atts);
                unset($this->lib_sources);
                unset($this->lib_name);
                break;
            case 'libname':
                $this->lib_name = $data;
                break;
            case 'maintainer':
                if (empty($this->pkginfo['maintainers'][$this->m_i]['role'])) {
                    $this->pkginfo['maintainers'][$this->m_i]['role'] = 'lead';
                }
                $this->m_i++;
                break;
            case 'release':
                if ($this->in_changelog) {
                    $this->c_i++;
                }
                break;
            case 'changelog':
                $this->in_changelog = false;
                break;
            case 'summary':
                $this->pkginfo['summary'] = $data;
                break;
        }
        array_pop($this->element_stack);
        $spos = sizeof($this->element_stack) - 1;
        $this->current_element = ($spos > 0) ? $this->element_stack[$spos] : '';
    }

    // }}}
    // {{{ _pkginfo_cdata_1_0()

    /**
     * XML parser callback for character data.  Used for version 1.0
     * packages.
     *
     * @param resource  XML parser resource
     * @param string    character data
     *
     * @return void
     *
     * @access private
     */
    function _pkginfo_cdata_1_0($xp, $data)
    {
        if (isset($this->cdata)) {
            $this->cdata .= $data;
        }
    }

    // }}}

    // {{{ infoFromTgzFile()

    /**
     * Returns information about a package file.  Expects the name of
     * a gzipped tar file as input.
     *
     * @param string   name of .tgz file
     *
     * @return array   array with package information
     *
     * @access public
     *
     */
    function infoFromTgzFile($file)
    {
        if (!@is_file($file)) {
            return $this->raiseError('tgz :: could not open file');
        }
        if (substr($file, -4) == '.tar') {
            $compress = false;
        } else {
            $compress = true;
        }
        $tar = new Archive_Tar($file, $compress);
        $content = $tar->listContent();
        if (!is_array($content)) {
            return $this->raiseError('tgz :: could not get contents of package');
        }
        $xml = null;
        foreach ($content as $file) {
            $name = $file['filename'];
            if ($name == 'package.xml') {
                $xml = $name;
            } elseif (ereg('^.*/package.xml$', $name, $match)) {
                $xml = $match[0];
            }
        }
        $tmpdir = System::mkTemp('-d pear');
        $this->addTempFile($tmpdir);
        if (!$xml || !$tar->extractList($xml, $tmpdir)) {
            return $this->raiseError('tgz :: could not extract the package.xml file');
        }
        return $this->infoFromDescriptionFile("$tmpdir/$xml");
    }

    // }}}
    // {{{ infoFromDescriptionFile()

    /**
     * Returns information about a package file.  Expects the name of
     * a package xml file as input.
     *
     * @param string   name of package xml file
     *
     * @return array   array with package information
     *
     * @access public
     *
     */
    function infoFromDescriptionFile($descfile)
    {
        if (!@is_file($descfile) || !is_readable($descfile) ||
             (!$fp = @fopen($descfile, 'r'))) {
            return $this->raiseError("Unable to open $descfile");
        }

        // read the whole thing so we only get one cdata callback
        // for each block of cdata
        $data = fread($fp, filesize($descfile));
        return $this->infoFromString($data);
    }

    // }}}
    // {{{ infoFromString()

    /**
     * Returns information about a package file.  Expects the contents
     * of a package xml file as input.
     *
     * @param string   name of package xml file
     *
     * @return array   array with package information
     *
     * @access public
     *
     */
    function infoFromString($data)
    {
        $xp = @xml_parser_create();
        if (!$xp) {
            return $this->raiseError('Unable to create XML parser');
        }
        xml_set_object($xp, $this);
        xml_set_element_handler($xp, '_element_start', '_element_end');
        xml_set_character_data_handler($xp, '_pkginfo_cdata');
        xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);

        $this->element_stack = array();
        $this->pkginfo = array();
        $this->current_element = false;
        $this->destdir = '';
        $this->pkginfo['filelist'] = array();
        $this->filelist =& $this->pkginfo['filelist'];
        $this->dir_names = array();
        $this->in_changelog = false;

        if (!xml_parse($xp, $data, 1)) {
            $code = xml_get_error_code($xp);
            $msg = sprintf("XML error: %s at line %d",
                           xml_error_string($code),
                           xml_get_current_line_number($xp));
            xml_parser_free($xp);
            return $this->raiseError($msg, $code);
        }

        xml_parser_free($xp);

        foreach ($this->pkginfo as $k => $v) {
            if (!is_array($v)) {
                $this->pkginfo[$k] = trim($v);
            }
        }
        return $this->pkginfo;
    }
    // }}}
    // {{{ xmlFromInfo()

    /**
     * Return an XML document based on the package info (as returned
     * by the PEAR_Common::infoFrom* methods).
     *
     * @param array package info
     *
     * @return string XML data
     *
     * @access public
     */
    function xmlFromInfo($pkginfo)
    {
        static $maint_map = array(
            "handle" => "user",
            "name" => "name",
            "email" => "email",
            "role" => "role",
            );
        $ret = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n";
        //$ret .= "<!DOCTYPE package SYSTEM \"http://pear.php.net/package10.dtd\">\n";
        $ret .= "<package version=\"1.0\">
  <name>$pkginfo[package]</name>
  <summary>".htmlspecialchars($pkginfo['summary'])."</summary>
  <description>".htmlspecialchars($pkginfo['description'])."</description>
  <maintainers>
";
        foreach ($pkginfo['maintainers'] as $maint) {
            $ret .= "    <maintainer>\n";
            foreach ($maint_map as $idx => $elm) {
                $ret .= "      <$elm>";
                $ret .= htmlspecialchars($maint[$idx]);
                $ret .= "</$elm>\n";
            }
            $ret .= "    </maintainer>\n";
        }
        $ret .= "  </maintainers>\n";
        $ret .= $this->_makeReleaseXml($pkginfo);
        if (@sizeof($pkginfo['changelog']) > 0) {
            $ret .= "  <changelog>\n";
            foreach ($pkginfo['changelog'] as $oldrelease) {
                $ret .= $this->_makeReleaseXml($oldrelease, true);
            }
            $ret .= "  </changelog>\n";
        }
        $ret .= "</package>\n";
        return $ret;
    }

    // }}}
    // {{{ _makeReleaseXml()

    /**
     * Generate part of an XML description with release information.
     *
     * @param array  array with release information
     * @param bool   whether the result will be in a changelog element
     *
     * @return string XML data
     *
     * @access private
     */
    function _makeReleaseXml($pkginfo, $changelog = false)
    {
        // XXX QUOTE ENTITIES IN PCDATA, OR EMBED IN CDATA BLOCKS!!
        $indent = $changelog ? "  " : "";
        $ret = "$indent  <release>\n";
        if (!empty($pkginfo['version'])) {
            $ret .= "$indent    <version>$pkginfo[version]</version>\n";
        }
        if (!empty($pkginfo['release_date'])) {
            $ret .= "$indent    <date>$pkginfo[release_date]</date>\n";
        }
        if (!empty($pkginfo['release_license'])) {
            $ret .= "$indent    <license>$pkginfo[release_license]</license>\n";
        }
        if (!empty($pkginfo['release_state'])) {
            $ret .= "$indent    <state>$pkginfo[release_state]</state>\n";
        }
        if (!empty($pkginfo['release_notes'])) {
            $ret .= "$indent    <notes>".htmlspecialchars($pkginfo['release_notes'])."</notes>\n";
        }
        if (isset($pkginfo['release_deps']) && sizeof($pkginfo['release_deps']) > 0) {
            $ret .= "$indent    <deps>\n";
            foreach ($pkginfo['release_deps'] as $dep) {
                $ret .= "$indent      <dep type=\"$dep[type]\" rel=\"$dep[rel]\"";
                if (isset($dep['version'])) {
                    $ret .= " version=\"$dep[version]\"";
                }
                if (isset($dep['name'])) {
                    $ret .= ">$dep[name]</dep>\n";
                } else {
                    $ret .= "/>\n";
                }
            }
            $ret .= "$indent    </deps>\n";
        }
        if (isset($pkginfo['filelist'])) {
            $ret .= "$indent    <filelist>\n";
            foreach ($pkginfo['filelist'] as $file => $fa) {
                if ($fa['role'] == 'extsrc') {
                    $ret .= "$indent      <libfile>\n";
                    $ret .= "$indent        <libname>$file</libname>\n";
                    $ret .= "$indent        <sources>$fa[sources]</sources>\n";
                    $ret .= "$indent      </libfile>\n";
                } else {
                    $ret .= "$indent      <file role=\"$fa[role]\"";
                    if (isset($fa['baseinstalldir'])) {
                        $ret .= " baseinstalldir=\"$fa[baseinstalldir]\"";
                    }
                    if (isset($fa['md5sum'])) {
                        $ret .= " md5sum=\"$fa[md5sum]\"";
                    }
                    $ret .= ">$file</file>\n";
                }
            }
            $ret .= "$indent    </filelist>\n";
        }
        $ret .= "$indent  </release>\n";
        return $ret;
    }

    // }}}
    // {{{ validatePackageInfo()

    function validatePackageInfo($info, &$errors, &$warnings)
    {
        if (is_string($info) && file_exists($info)) {
            $tmp = substr($info, -4);
            if ($tmp == '.xml') {
                $info = $this->infoFromDescriptionFile($info);
            } elseif ($tmp == '.tar' || $tmp == '.tgz') {
                $info = $this->infoFromTgzFile($info);
            } else {
                $fp = fopen($params[0], "r");
                $test = fread($fp, 5);
                fclose($fp);
                if ($test == "<?xml") {
                    $info = $obj->infoFromDescriptionFile($params[0]);
                } else {
                    $info = $obj->infoFromTgzFile($params[0]);
                }
            }
            if (PEAR::isError($info)) {
                return $this->raiseError($info);
            }
        }
        if (!is_array($info)) {
            return false;
        }
        $errors = array();
        $warnings = array();
        if (empty($info['package'])) {
            $errors[] = 'missing package name';
        }
        if (empty($info['summary'])) {
            $errors[] = 'missing summary';
        } elseif (strpos(trim($info['summary']), "\n") !== false) {
            $warnings[] = 'summary should be on a single line';
        }
        if (empty($info['description'])) {
            $errors[] = 'missing description';
        }
        if (empty($info['release_license'])) {
            $errors[] = 'missing license';
        }
        if (empty($info['version'])) {
            $errors[] = 'missing version';
        }
        if (empty($info['release_state'])) {
            $errors[] = 'missing release state';
        } elseif (!in_array($info['release_state'], $this->release_states)) {
            $errors[] = "invalid release state `$info[release_state]', should be one of: ".implode(' ', $this->release_states);
        }
        if (empty($info['release_date'])) {
            $errors[] = 'missing release date';
        } elseif (!preg_match('/^\d{4}-\d\d-\d\d$/', $info['release_date'])) {
            $errors[] = "invalid release date `$info[release_date]', format is YYYY-MM-DD";
        }
        if (empty($info['release_notes'])) {
            $errors[] = "missing release notes";
        }
        if (empty($info['maintainers'])) {
            $errors[] = 'no maintainer(s)';
        } else {
            $i = 1;
            foreach ($info['maintainers'] as $m) {
                if (empty($m['handle'])) {
                    $errors[] = "maintainer $i: missing handle";
                }
                if (empty($m['role'])) {
                    $errors[] = "maintainer $i: missing role";
                } elseif (!in_array($m['role'], $this->maintainer_roles)) {
                    $errors[] = "maintainer $i: invalid role `$m[role]', should be one of: ".implode(' ', $this->maintainer_roles);
                }
                if (empty($m['name'])) {
                    $errors[] = "maintainer $i: missing name";
                }
                if (empty($m['email'])) {
                    $errors[] = "maintainer $i: missing email";
                }
                $i++;
            }
        }
        if (!empty($info['deps'])) {
            $i = 1;
            foreach ($info['deps'] as $d) {
                if (empty($d['type'])) {
                    $errors[] = "depenency $i: missing type";
                } elseif (!in_array($d['type'], $this->dependency_types)) {
                    $errors[] = "depenency $i: invalid type, should be one of: ".implode(' ', $this->depenency_types);
                }
                if (empty($d['rel'])) {
                    $errors[] = "dependency $i: missing relation";
                } elseif (!in_array($d['rel'], $this->dependency_relations)) {
                    $errors[] = "dependency $i: invalid relation, should be one of: ".implode(' ', $this->dependency_relations);
                }
                if ($d['rel'] != 'has' && empty($d['version'])) {
                    $warnings[] = "dependency $i: missing version";
                } elseif ($d['rel'] == 'has' && !empty($d['version'])) {
                    $warnings[] = "dependency $i: version ignored for `has' dependencies";
                }
                if ($d['type'] == 'php' && !empty($d['name'])) {
                    $warnings[] = "dependency $i: name ignored for php type dependencies";
                } elseif ($d['type'] != 'php' && empty($d['name'])) {
                    $errors[] = "dependency $i: missing name";
                }
                $i++;
            }
        }
        if (empty($info['filelist'])) {
            $errors[] = 'no files';
        } else {
            foreach ($info['filelist'] as $file => $fa) {
                if (empty($fa['role'])) {
                    $errors[] = "$file: missing role";
                } elseif (!in_array($fa['role'], $this->file_roles)) {
                    $errors[] = "$file: invalid role, should be one of: ".implode(' ', $this->file_roles);
                } elseif ($fa['role'] == 'extsrc' && empty($fa['sources'])) {
                    $errors[] = "$file: no source files";
                }
                // Any checks we can do for baseinstalldir?
            }
        }
        return true;
    }

    // }}}
}
?>