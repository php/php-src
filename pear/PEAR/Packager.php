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
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Common.php';

/**
 * Administration class used to make a PEAR release tarball.
 *
 * TODO:
 *  - add an extra param the dir where to place the created package
 *  - finish and test Windows support
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Packager extends PEAR_Common
{
    // {{{ properties

    /** assoc with information about the package */
    var $pkginfo = array();

    /** name of the package directory, for example Foo-1.0 */
    var $pkgdir;

    /** directory where PHP code files go */
    var $phpdir;

    /** directory where PHP extension files go */
    var $extdir;

    /** directory where documentation goes */
    var $docdir;

    /** directory where system state information goes */
    var $statedir;

    /** debug mode (integer) */
    var $debug = 0;

    /** temporary directory */
    var $tmpdir;

    /** whether file list is currently being copied */
    var $recordfilelist;

    /** temporary space for copying file list */
    var $filelist;

    /** package name and version, for example "HTTP-1.0" */
    var $pkgver;

    // }}}

    // {{{ constructor

    function PEAR_Packager($phpdir = PEAR_INSTALL_DIR,
                           $extdir = PEAR_EXTENSION_DIR,
                           $docdir = '')
    {
        $this->PEAR();
        $this->phpdir = $phpdir;
        $this->extdir = $extdir;
        $this->docdir = $docdir;
    }

    // }}}
    // {{{ destructor

    function _PEAR_Packager()
    {
        chdir($this->orig_pwd);
        $this->_PEAR_Common();
    }

    // }}}

    // {{{ package()

    function package($pkgfile = null)
    {
        $this->orig_pwd = getcwd();
        if (empty($pkgfile)) {
            $pkgfile = 'package.xml';
        }
        $pkginfo = $this->infoFromDescriptionFile($pkgfile);
        if (PEAR::isError($pkginfo)) {
            return $pkginfo;
        }
        // TMP DIR -------------------------------------------------
        // We allow calls like "pear package /home/user/mypack/package.xml"
        if (!@chdir(dirname($pkgfile))) {
            return $this->raiseError('Couldn\'t chdir to package.xml dir',
                              null, PEAR_ERROR_TRIGGER, E_USER_ERROR);
        }
        $pwd = getcwd();
        $pkgfile = basename($pkgfile);
        // don't want strange characters
        $pkgname    = ereg_replace ('[^a-zA-Z0-9._]', '_', $pkginfo['package']);
        $pkgversion = ereg_replace ('[^a-zA-Z0-9._\-]', '_', $pkginfo['version']);
        $pkgver = $pkgname . '-' . $pkgversion;

        $tmpdir = $pwd . DIRECTORY_SEPARATOR . $pkgver;
        if (file_exists($tmpdir)) {
            return $this->raiseError('Tmpdir: ' . $tmpdir .' already exists',
                              null, PEAR_ERROR_TRIGGER, E_USER_ERROR);
        }
        if (!mkdir($tmpdir, 0700)) {
            return $this->raiseError("Unable to create temporary directory $tmpdir.",
                              null, PEAR_ERROR_TRIGGER, E_USER_ERROR);
        } else {
            $this->log(2, "+ tmp dir created at: " . $tmpdir);
        }
        $this->addTempFile($tmpdir);

        // Copy files -----------------------------------------------
        foreach ($pkginfo['filelist'] as $fname => $atts) {
            $file = $tmpdir . DIRECTORY_SEPARATOR . $fname;
            $dir = dirname($file);
            if (!@is_dir($dir)) {
                if (!$this->mkDirHier($dir)) {
                    return $this->raiseError("could not mkdir $dir");
                }
            }
            //Maintain original file perms
            $orig_perms = @fileperms($fname);
            if (!@copy($fname, $file)) {
                $this->log(0, "could not copy $fname to $file");
            } else {
                $this->log(2, "+ copying $fname to $file");
                @chmod($file, $orig_perms);
            }
        }
        // XXX TODO: Rebuild the package file as the old method did?

        // This allows build packages from different pear pack def files
        $dest_pkgfile = $tmpdir . DIRECTORY_SEPARATOR . 'package.xml';
        $this->log(2, "+ copying package $pkgfile to $dest_pkgfile");
        if (!@copy($pkgfile, $dest_pkgfile)) {
            return $this->raiseError("could not copy $pkgfile to $dest_pkgfile");
        }
        @chmod($dest_pkgfile, 0644);

        // TAR the Package -------------------------------------------
        chdir(dirname($tmpdir));
        $dest_package = $this->orig_pwd . DIRECTORY_SEPARATOR . "{$pkgver}.tgz";
        $this->log(2, "Attempting to pack $tmpdir dir in $dest_package");
        $tar = new Archive_Tar($dest_package, true);
        $tar->setErrorHandling(PEAR_ERROR_PRINT);
        if (!$tar->create($pkgver)) {
            return $this->raiseError('an error ocurred during package creation');
        }
        $this->log(1, "Package $dest_package done");
        return $dest_package;
    }

    /* XXXX REMOVEME: this is the old code
    function _package($pkgfile = "package.xml")
    {
        $pwd = getcwd();
        $fp = @fopen($pkgfile, "r");
        if (!is_resource($fp)) {
            return $this->raiseError($php_errormsg);
        }

        $xp = xml_parser_create();
        if (!$xp) {
            return $this->raiseError("Unable to create XML parser.");
        }
        xml_set_object($xp, $this);
        xml_set_element_handler($xp, "startHandler", "endHandler");
        xml_set_character_data_handler($xp, "charHandler");
        xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);
        xml_parser_set_option($xp, XML_OPTION_TARGET_ENCODING, "UTF-8");

        $this->element_stack = array();
        $this->pkginfo = array();
        $this->current_element = false;

        $data = fread($fp, filesize($pkgfile));
        fclose($fp);
        if (!xml_parse($xp, $data, true)) {
            $msg = sprintf("XML error: %s at line %d",
                           xml_error_string(xml_get_error_code($xp)),
                           xml_get_current_line_number($xp));
            xml_parser_free($xp);
            return $this->raiseError($msg);
        }
        xml_parser_free($xp);

        $pkginfofile = $this->tmpdir . DIRECTORY_SEPARATOR . "package.xml";
        $fp = fopen($pkginfofile, "w");
        if (!is_resource($fp)) {
            return $this->raiseError("Could not create $pkginfofile: $php_errormsg");
        }

        $this->filelist = preg_replace('/^[\r\n]+\s+/', '    ', $this->filelist);
        $this->filelist = preg_replace('/\n\s+/', "\n    ", $this->filelist);
        $this->filelist = preg_replace('/\n\s+$/', "", $this->filelist);

        fputs($fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n".
              "<!DOCTYPE Package PUBLIC \"-//PHP Group//DTD PEAR Package 1.0//EN//XML\" \"http://php.net/pear/package.dtd\">\n".
              "<Package Type=\"Source\">\n".
              "  <Name>".$this->pkginfo["Package,Name"]."</Name>\n".
              "  <Summary>".$this->pkginfo["Package,Summary"]."</Summary>\n".
              "  <Maintainer>\n".
              "    <Initials>".$this->pkginfo["Maintainer,Initials"]."</Initials>\n".
              "    <Name>".$this->pkginfo["Maintainer,Name"]."</Name>\n".
              "    <Email>".$this->pkginfo["Maintainer,Email"]."</Email>\n".
              "  </Maintainer>\n".
              "  <Release>\n".
              "    <Version>".$this->pkginfo["Release,Version"]."</Version>\n".
              "    <Date>".$this->pkginfo["Release,Date"]."</Date>\n".
              "    <Notes>".$this->pkginfo["Release,Notes"]."</Notes>\n".
              "  </Release>\n".
              "  <FileList>\n".
              "$this->filelist\n".
              "  </FileList>\n".
              "</Package>\n");
        fclose($fp);
        chdir(dirname($this->tmpdir));
        // XXX FIXME Windows and non-GNU tar
        $pkgver = $this->pkgver;
        $cmd = "tar -cvzf $pwd/${pkgver}.tgz $pkgver";
        $this->log(1, `$cmd`);
        $this->log(1, "Package $pwd/${pkgver}.tgz done");
    }

    // }}}

    // {{{ startHandler()

    function startHandler($xp, $name, $attribs)
    {
        array_push($this->element_stack, $name);
        $this->current_element = $name;
        $this->current_attributes = $attribs;
        $this->tmpdata = '';
        if ($this->recordfilelist) {
            $this->filelist .= "<$name";
            foreach ($attribs as $k => $v) {
                $this->filelist .= " $k=\"$v\"";
            }
            $this->filelist .= ">";
        }
        switch ($name) {
            case "Package":
                if ($attribs["Type"]) {
                    // warning
                }
                break;
            case "FileList":
                // XXX FIXME Windows
                $this->recordfilelist = true;
                $pwd = getcwd();
                $this->pkgver = $this->pkginfo["Package,Name"] . "-" .
                    $this->pkginfo["Release,Version"];
                // don't want extrange characters
                $this->pkgver = ereg_replace ("[^a-zA-Z0-9._-]", '_', $this->pkgver);
                $this->tmpdir = $pwd . DIRECTORY_SEPARATOR . $this->pkgver;
                if (file_exists($this->tmpdir)) {
                    xml_parser_free($xp);
                    $this->raiseError("$this->tmpdir already exists",
                                      null, PEAR_ERROR_TRIGGER,
                                      E_USER_ERROR);
                }
                if (!mkdir($this->tmpdir, 0755)) {
                    xml_parser_free($xp);
                    $this->raiseError("Unable to create temporary directory $this->tmpdir.",
                                      null, PEAR_ERROR_TRIGGER,
                                      E_USER_ERROR);
                }
                $this->_tempfiles[] = $this->tmpdir;
                break;
        }
    }

    // }}}
    // {{{ endHandler()

    function endHandler($xp, $name)
    {
        array_pop($this->element_stack);
        $this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
        switch ($name) {
            case "FileList":
                $this->recordfilelist = false;
                break;
        }
        if ($this->recordfilelist) {
            $this->filelist .= "</$name>";
        }
    }

    // }}}
    // {{{ charHandler()

    function charHandler($xp, $data)
    {
        if ($this->recordfilelist) {
            $this->filelist .= $data;
        }
        switch ($this->current_element) {
            case "Dir":
                break;
            case "File":
                $file = "$this->tmpdir/$data";
                $dir = dirname($file);
                if (!is_dir($dir)) {
                    if (!$this->mkDirHier($dir)) {
                        $this->log(0, "could not mkdir $dir");
                        break;
                    }
                }
                if (!@copy($data, $file)) {
                    $this->log(0, "could not copy $data to $file");
                }
                // fall through
            default:
                $data = trim($data);
                if ($data) {
                    $id = implode(",", array_slice($this->element_stack, -2));
                    $this->pkginfo[$id] = $data;
                }
                break;
        }
    }
    */
    // }}}
}

?>
