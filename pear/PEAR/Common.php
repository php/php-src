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
 * TODO:
 *   - check in inforFromDescFile that the minimal data needed is present
 *     (pack name, version, files, others?)
 *   - inherance of dir attribs to files may fail under certain circumstances
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

    /** list of temporary files created by this object */
    var $_tempfiles = array();

    /** assoc with information about a package */
    var $pkginfo = array();

    /**
     * Permitted maintainer roles
     * @var array
     */
    var $maintainer_roles = array('lead','developer','contributor','helper');

    /**
     * Permitted release states
     * @var array
     */
    var $releases_states  = array('alpha','beta','stable','snapshot');

    // }}}

    // {{{ constructor

    function PEAR_Common()
    {
        $GLOBALS['_PEAR_Common_tempfiles'] = array();
        $this->_tempfiles =& $GLOBALS['_PEAR_Common_tempfiles'];
        $this->PEAR();
    }

    // }}}
    // {{{ destructor

    function _PEAR_Common()
    {
        // doesn't work due to bug #14744
        //$tempfiles = $this->_tempfiles;
        $tempfiles =& $GLOBALS['_PEAR_Common_tempfiles'];
        while (is_array($tempfiles) &&
               $file = array_shift($tempfiles))
        {
            if (@is_dir($file)) {
                System::rm("-rf $file");
            } elseif (file_exists($file)) {
                unlink($file);
            }
        }
    }

    // }}}
    // {{{ addTempFile()

    function addTempFile($file)
    {
        $this->_tempfiles[] = $file;
    }

    // }}}
    // {{{ mkDirHier()

    function mkDirHier($dir)
    {
        $this->log(2, "+ create dir $dir");
        return System::mkDir("-p $dir");
    }

    // }}}
    // {{{ log()

    function log($level, $msg)
    {
        if ($this->debug >= $level) {
            print "$msg\n";
        }
    }

    // }}}
    // {{{ mkTempDir()

    function mkTempDir()
    {
        if (!$tmpdir = System::mktemp('-d pear')) {
            return false;
        }
        $this->addTempFile($tmpdir);
        return $tmpdir;
    }

    // }}}
    // {{{ _element_start()

    function _element_start($xp, $name, $attribs)
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
                $this->lib_atts['role'] = 'extension';
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
                $this->pkginfo['release_deps'] = array();
                break;
            case 'dep':
                // dependencies array index
                $this->d_i = (isset($this->d_i)) ? $this->d_i + 1 : 0;
                $this->pkginfo['release_deps'][$this->d_i] = $attribs;
                break;
        }
    }

    // }}}
    // {{{ _element_end()

    function _element_end($xp, $name)
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
                if (!in_array($data, $this->maintainer_roles)) {
                    trigger_error("The maintainer role: '$data' is not valid", E_USER_WARNING);
                } else {
                    $this->current_maintainer['role'] = $data;
                }
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
                if (!in_array($data, $this->releases_states)) {
                    trigger_error("The release state: '$data' is not valid", E_USER_WARNING);
                } elseif ($this->in_changelog) {
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
                $this->lib_name = $data;
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
                    $this->filelist[$path]['sources'] = $this->lib_sources;
                }
                unset($this->lib_atts);
                unset($this->lib_sources);
                break;
            case 'maintainer':
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
    // {{{ _pkginfo_cdata()

    function _pkginfo_cdata($xp, $data)
    {
        $this->cdata .= $data;
    }

    // }}}
    // {{{ infoFromDescriptionFile()

    function infoFromDescriptionFile($descfile)
    {
        if (!@is_file($descfile) || !is_readable($descfile) ||
             (!$fp = @fopen($descfile, 'r'))) {
            return $this->raiseError("Unable to open $descfile");
        }
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

        // read the whole thing so we only get one cdata callback
        // for each block of cdata
        $data = fread($fp, filesize($descfile));
        if (!xml_parse($xp, $data, 1)) {
            $msg = sprintf("XML error: %s at line %d",
                           xml_error_string(xml_get_error_code($xp)),
                           xml_get_current_line_number($xp));
            xml_parser_free($xp);
            return $this->raiseError($msg);
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
    // {{{ infoFromTgzFile()

    /**
    * Returns info from a tgz pear package
    */
    function infoFromTgzFile($file)
    {
        if (!@is_file($file)) {
            return $this->raiseError('tgz :: could not open file');
        }
        $tar = new Archive_Tar($file, true);
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

    // }}}
    }
}
?>