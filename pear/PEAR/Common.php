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

require_once "PEAR.php";

class PEAR_Common extends PEAR
{
    // {{{ properties

    /** stack of elements, gives some sort of XML context */
    var $element_stack;

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

    function _PEAR_Common() {
	$this->_PEAR();
        while (is_array($this->_tempfiles) &&
               $file = array_shift($this->_tempfiles))
        {
            if (is_dir($file)) {
                system("rm -rf $file"); // XXX FIXME Windows
            } else {
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
    // {{{ _element_start()

    function _element_start($xp, $name, $attribs)
    {
	array_push($this->element_stack, $name);
	$this->current_element = $name;
	$this->current_attributes = $attribs;
    }

    // }}}
    // {{{ _element_end()

    function _element_end($xp, $name)
    {
	array_pop($this->element_stack);
	$this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
    }

    // }}}
    // {{{ _pkginfo_cdata()

    function _pkginfo_cdata($xp, $data)
    {
        $next = $this->element_stack[sizeof($this->element_stack)-1];
	switch ($this->current_element) {
            case "Name":
                switch ($next) {
                    case "Package":
                        $this->pkginfo["package"] .= $data;
                        break;
                    case "Maintainer":
                        $this->pkginfo["maintainer_name"] .= $data;
                        break;
                }
                break;
            case "Summary":
                $this->pkginfo["summary"] .= $data;
                break;
            case "Initials":
                $this->pkginfo["maintainer_handle"] .= $data;
                break;
            case "Email":
                $this->pkginfo["maintainer_email"] .= $data;
                break;
            case "Version":
                $this->pkginfo["version"] .= $data;
                break;
            case "Date":
                $this->pkginfo["release_date"] .= $data;
                break;
            case "Notes":
                $this->pkginfo["release_notes"] .= $data;
                break;
	    case "Dir":
		if (!$this->phpdir) {
		    break;
		}
                $dir = trim($data);
                // XXX add to file list
		break;
	    case "File":
                $role = strtolower($this->current_attributes["Role"]);
		$file = trim($data);
                // XXX add to file list
		break;
	}
    }

    // }}}
    // {{{ infoFromDescriptionFile()

    function infoFromDescriptionFile($descfile)
    {
	$fp = fopen($descfile, "r");
	$xp = @xml_parser_create();
	if (!$xp) {
	    return $this->raiseError("Unable to create XML parser.");
	}
	xml_set_object($xp, $this);
	xml_set_element_handler($xp, "_element_start", "_element_end");
	xml_set_character_data_handler($xp, "_pkginfo_cdata");
	xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);

	$this->element_stack = array();
	$this->pkginfo = array();
	$this->current_element = false;
	$this->destdir = '';

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

        return $this->pkginfo;
    }

    // }}}
}

?>
