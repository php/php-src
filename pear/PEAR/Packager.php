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

require_once 'PEAR/Common.php';

/**
 * Administration class used to make a PEAR release tarball.
 *
 * TODO:
 *  - add an extra param the dir where to place the created package
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Packager extends PEAR_Common
{
    // {{{ properties

    /** debug mode (integer) */
    var $debug = 0;

    // }}}

    // {{{ constructor

    function PEAR_Packager()
    {
        $this->PEAR();
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
        // XXX This needs to be checked in infoFromDescriptionFile
        //     or at least a helper method to do the proper checks
        if (empty($pkginfo['version'])) {
            return $this->raiseError("No version info found in $pkgfile");
        }
        // TMP DIR -------------------------------------------------
        // We allow calls like "pear package /home/user/mypack/package.xml"
        if (!@chdir(dirname($pkgfile))) {
            return $this->raiseError('Could not chdir to '.dirname($pkgfile));
        }
        $pwd = getcwd();
        $pkgfile = basename($pkgfile);
        if (isset($pkginfo['release_state']) && $pkginfo['release_state'] == 'snapshot' && empty($pkginfo['version'])) {
            $pkginfo['version'] = date('Ymd');
        }
        // don't want strange characters
        $pkgname    = preg_replace('/[^a-z0-9._]/i', '_', $pkginfo['package']);
        $pkgversion = preg_replace('/[^a-z0-9._-]/i', '_', $pkginfo['version']);
        $pkgver = $pkgname . '-' . $pkgversion;

        // ----- Create the package file list
        $filelist = array();
        $i = 0;

        // Copy files -----------------------------------------------
        foreach ($pkginfo['filelist'] as $fname => $atts) {
            if (!file_exists($fname)) {
                return $this->raiseError("File $fname does not exist");
            } else {
                $filelist[$i++] = $fname;
                if (empty($pkginfo['filelist'][$fname]['md5sum'])) {
                    $md5sum = md5_file($fname);
                    $pkginfo['filelist'][$fname]['md5sum'] = $md5sum;
                }
                $this->log(2, "Adding file $fname");
            }
        }
        $new_xml = $this->xmlFromInfo($pkginfo);
        if (PEAR::isError($new_xml)) {
            return $new_xml;
        }
        $tmpdir = $this->mkTempDir(getcwd());
        $newpkgfile = $tmpdir . DIRECTORY_SEPARATOR . $pkgfile;
        $np = @fopen($newpkgfile, "w");
        if (!$np) {
            return $this->raiseError("PEAR_Packager: unable to rewrite $pkgfile");
        }
        fwrite($np, $new_xml);
        fclose($np);

        // TAR the Package -------------------------------------------
        $dest_package = $this->orig_pwd . DIRECTORY_SEPARATOR . "{$pkgver}.tgz";
        $tar =& new Archive_Tar($dest_package, true);
        $tar->setErrorHandling(PEAR_ERROR_RETURN); // XXX Don't print errors
        // ----- Creates with the package.xml file
        $ok = $tar->createModify($newpkgfile, '', $tmpdir);
        if (PEAR::isError($ok)) {
            return $ok;
        } elseif (!$ok) {
            return $this->raiseError('PEAR_Packager: tarball creation failed');
        }
        // ----- Add the content of the package
        if (!$tar->addModify($filelist, $pkgver)) {
            return $this->raiseError('PEAR_Packager: tarball creation failed');
        }
        $this->log(1, "Package $dest_package done");
        $cvsversion = preg_replace('/[^a-z0-9]/i', '_', $pkgversion);
        $cvstag = "RELEASE_$cvsversion";
        $this->log(1, "CVS release tag: $cvstag");
        return $dest_package;
    }

    // }}}
}

?>