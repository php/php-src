<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
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
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';

/**
* TODO:
*   - check in inforFromDescFile that the minimal data needed is present
*     (pack name, version, files, others?)
*   - perhaps use parser folding to be less restrictive with the format
      of the package.xml file
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

    // }}}

    // {{{ constructor

    function PEAR_Common()
    {
        $this->PEAR();
    }

    // }}}
    // {{{ destructor

    function _PEAR_Common()
    {
        $this->_PEAR();
        while (is_array($this->_tempfiles) &&
               $file = array_shift($this->_tempfiles))
        {
            if (@is_dir($file)) {
                system("rm -rf $file"); // XXX FIXME Windows
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
        $dirstack = array();
        while (!@is_dir($dir) && $dir != DIRECTORY_SEPARATOR) {
            array_unshift($dirstack, $dir);
            $dir = dirname($dir);
        }
        while ($newdir = array_shift($dirstack)) {
            if (mkdir($newdir, 0777)) {
                $this->log(2, "+ created dir $newdir");
            } else {
                return false;
            }
        }
        return true;
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

    // {{{ _element_start()

    function _element_start($xp, $name, $attribs)
    {
        array_push($this->element_stack, $name);
        $this->current_element = $name;
        $this->current_attributes = $attribs;
        switch ($name) {
            case 'Dir':
                if (isset($this->dir_names)) {
                    $this->dir_names[] = $attribs['Name'];
                } else {
                    // Don't add the root dir
                    $this->dir_names = array();
                }
                if (isset($attribs['BaseInstallDir'])) {
                    $this->dir_install = $attribs['BaseInstallDir'];
                }
                if (isset($attribs['Role'])) {
                    $this->dir_role = $attribs['Role'];
                }
                break;
            case 'LibFile':
                $this->lib_atts = $attribs;
                $this->lib_atts['Role'] = 'extension';
                break;
        }
    }

    // }}}
    // {{{ _element_end()

    function _element_end($xp, $name)
    {
        switch ($name) {
            case 'Dir':
                array_pop($this->dir_names);
                unset($this->dir_install);
                unset($this->dir_role);
                break;
            case 'File':
                $path = '';
                foreach ($this->dir_names as $dir) {
                    $path .= $dir . DIRECTORY_SEPARATOR;
                }
                $path .= $this->current_file;
                $this->filelist[$path] = $this->current_attributes;
                // Set the baseinstalldir only if the file don't have this attrib
                if (!isset($this->filelist[$path]['BaseInstallDir']) &&
                    isset($this->dir_install))
                {
                    $this->filelist[$path]['BaseInstallDir'] = $this->dir_install;
                }
                // Set the Role
                if (!isset($this->filelist[$path]['Role']) && isset($this->dir_role)) {
                    $this->filelist[$path]['Role'] = $this->dir_role;
                }
                break;
            case 'LibFile':
                $path = '';
                foreach ($this->dir_names as $dir) {
                    $path .= $dir . DIRECTORY_SEPARATOR;
                }
                $path .= $this->lib_name;
                $this->filelist[$path] = $this->lib_atts;
                // Set the baseinstalldir only if the file don't have this attrib
                if (!isset($this->filelist[$path]['BaseInstallDir']) &&
                    isset($this->dir_install))
                {
                    $this->filelist[$path]['BaseInstallDir'] = $this->dir_install;
                }
                if (isset($this->lib_sources)) {
                    $this->filelist[$path]['sources'] = $this->lib_sources;
                }
                unset($this->lib_atts);
                unset($this->lib_sources);
                break;
        }
        array_pop($this->element_stack);
        $this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
    }

    // }}}
    // {{{ _pkginfo_cdata()

    function _pkginfo_cdata($xp, $data)
    {
        $prev = $this->element_stack[sizeof($this->element_stack)-2];
        switch ($this->current_element) {
            case 'Name':
                switch ($prev) {
                    case 'Package':
                        $this->pkginfo['package'] .= $data;
                        break;
                    case 'Maintainer':
                        $this->pkginfo['maintainer_name'] .= $data;
                        break;
                }
                break;
            case 'Summary':
                $this->pkginfo['summary'] .= $data;
                break;
            case 'Initials':
                $this->pkginfo['maintainer_handle'] .= $data;
                break;
            case 'Email':
                $this->pkginfo['maintainer_email'] .= $data;
                break;
            case 'Version':
                $this->pkginfo['version'] .= $data;
                break;
            case 'Date':
                $this->pkginfo['release_date'] .= $data;
                break;
            case 'Notes':
                $this->pkginfo['release_notes'] .= $data;
                break;
            case 'Dir':
                if (!$this->phpdir) {
                    break;
                }
                $dir = trim($data);
                // XXX add to file list
                break;
            case 'File':
                $role = strtolower($this->current_attributes['Role']);
                $this->current_file = trim($data);
                break;
            case 'LibName':
                $this->lib_name = trim($data);
                break;
            case 'Sources':
                $this->lib_sources[] = trim($data);
                break;
        }
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
        $this->filelist = array();

        // read the whole thing so we only get one cdata callback
        // for each block of cdata
        $data = fread($fp, filesize($descfile));
        if (!@xml_parse($xp, $data, 1)) {
            $msg = sprintf("XML error: %s at line %d",
                           xml_error_string(xml_get_error_code($xp)),
                           xml_get_current_line_number($xp));
            xml_parser_free($xp);
            return $this->raiseError($msg);
        }

        xml_parser_free($xp);

        foreach ($this->pkginfo as $k => $v) {
            $this->pkginfo[$k] = trim($v);
        }
        $this->pkginfo['filelist'] = &$this->filelist;
        return $this->pkginfo;
    }

    // }}}
}
?>