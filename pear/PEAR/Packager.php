<?php
/**
 * PEAR_Packager for generating releases
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
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V. V. Cox <cox@idecnet.com>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * base class
 */
require_once 'PEAR/Common.php';
require_once 'PEAR/PackageFile.php';
require_once 'System.php';

/**
 * Administration class used to make a PEAR release tarball.
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */
class PEAR_Packager extends PEAR_Common
{
    /**
     * @var PEAR_Registry
     */
    var $_registry;
    // {{{ package()

    function package($pkgfile = null, $compress = true, $pkg2 = null)
    {
        // {{{ validate supplied package.xml file
        if (empty($pkgfile)) {
            $pkgfile = 'package.xml';
        }
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $pkg = &new PEAR_PackageFile($this->config, $this->debug);
        $pf = &$pkg->fromPackageFile($pkgfile, PEAR_VALIDATE_NORMAL);
        $main = &$pf;
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($pf)) {
            if (is_array($pf->getUserInfo())) {
                foreach ($pf->getUserInfo() as $error) {
                    $this->log(0, 'Error: ' . $error['message']);
                }
            }
            $this->log(0, $pf->getMessage());
            return $this->raiseError("Cannot package, errors in package file");
        } else {
            foreach ($pf->getValidationWarnings() as $warning) {
                $this->log(1, 'Warning: ' . $warning['message']);
            }
        }

        // }}}
        if ($pkg2) {
            $this->log(0, 'Attempting to process the second package file');
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $pf2 = &$pkg->fromPackageFile($pkg2, PEAR_VALIDATE_NORMAL);
            PEAR::staticPopErrorHandling();
            if (PEAR::isError($pf2)) {
                if (is_array($pf2->getUserInfo())) {
                    foreach ($pf2->getUserInfo() as $error) {
                        $this->log(0, 'Error: ' . $error['message']);
                    }
                }
                $this->log(0, $pf2->getMessage());
                return $this->raiseError("Cannot package, errors in second package file");
            } else {
                foreach ($pf2->getValidationWarnings() as $warning) {
                    $this->log(1, 'Warning: ' . $warning['message']);
                }
            }
            if ($pf2->getPackagexmlVersion() == '2.0') {
                $main = &$pf2;
                $other = &$pf;
            } else {
                $main = &$pf;
                $other = &$pf2;
            }
            if ($main->getPackagexmlVersion() != '2.0') {
                return PEAR::raiseError('Error: cannot package two package.xml version 1.0, can ' .
                    'only package together a package.xml 1.0 and package.xml 2.0');
            }
            if ($other->getPackagexmlVersion() != '1.0') {
                return PEAR::raiseError('Error: cannot package two package.xml version 2.0, can ' .
                    'only package together a package.xml 1.0 and package.xml 2.0');
            }
        }
        $main->setLogger($this);
        if (!$main->validate(PEAR_VALIDATE_PACKAGING)) {
            foreach ($main->getValidationWarnings() as $warning) {
                $this->log(0, 'Error: ' . $warning['message']);
            }
            return $this->raiseError("Cannot package, errors in package");
        } else {
            foreach ($main->getValidationWarnings() as $warning) {
                $this->log(1, 'Warning: ' . $warning['message']);
            }
        }
        if ($pkg2) {
            $other->setLogger($this);
            $a = false;
            if (!$other->validate(PEAR_VALIDATE_NORMAL) || $a = !$main->isEquivalent($other)) {
                foreach ($other->getValidationWarnings() as $warning) {
                    $this->log(0, 'Error: ' . $warning['message']);
                }
                foreach ($main->getValidationWarnings() as $warning) {
                    $this->log(0, 'Error: ' . $warning['message']);
                }
                if ($a) {
                    return $this->raiseError('The two package.xml files are not equivalent!');
                }
                return $this->raiseError("Cannot package, errors in package");
            } else {
                foreach ($other->getValidationWarnings() as $warning) {
                    $this->log(1, 'Warning: ' . $warning['message']);
                }
            }
            $gen = &$main->getDefaultGenerator();
            $tgzfile = $gen->toTgz2($this, $other, $compress);
            if (PEAR::isError($tgzfile)) {
                return $tgzfile;
            }
            $dest_package = basename($tgzfile);
            $pkgdir = dirname($pkgfile);
    
            // TAR the Package -------------------------------------------
            $this->log(1, "Package $dest_package done");
            if (file_exists("$pkgdir/CVS/Root")) {
                $cvsversion = preg_replace('/[^a-z0-9]/i', '_', $pf->getVersion());
                $cvstag = "RELEASE_$cvsversion";
                $this->log(1, 'Tag the released code with "pear cvstag ' .
                    $main->getPackageFile() . '"');
                $this->log(1, "(or set the CVS tag $cvstag by hand)");
            }
        } else { // this branch is executed for single packagefile packaging
            $gen = &$pf->getDefaultGenerator();
            $tgzfile = $gen->toTgz($this, $compress);
            if (PEAR::isError($tgzfile)) {
                $this->log(0, $tgzfile->getMessage());
                return $this->raiseError("Cannot package, errors in package");
            }
            $dest_package = basename($tgzfile);
            $pkgdir = dirname($pkgfile);
    
            // TAR the Package -------------------------------------------
            $this->log(1, "Package $dest_package done");
            if (file_exists("$pkgdir/CVS/Root")) {
                $cvsversion = preg_replace('/[^a-z0-9]/i', '_', $pf->getVersion());
                $cvstag = "RELEASE_$cvsversion";
                $this->log(1, "Tag the released code with `pear cvstag $pkgfile'");
                $this->log(1, "(or set the CVS tag $cvstag by hand)");
            }
        }
        return $dest_package;
    }

    // }}}
}

// {{{ md5_file() utility function
if (!function_exists('md5_file')) {
    function md5_file($file) {
        if (!$fd = @fopen($file, 'r')) {
            return false;
        }
        if (function_exists('file_get_contents')) {
            fclose($fd);
            $md5 = md5(file_get_contents($file));
        } else {
            $md5 = md5(fread($fd, filesize($file)));
            fclose($fd);
        }
        return $md5;
    }
}
// }}}

?>
