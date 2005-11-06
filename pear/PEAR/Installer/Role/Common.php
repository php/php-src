<?php
/**
 * Base class for all installation roles.
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
 * Base class for all installation roles.
 *
 * This class allows extensibility of file roles.  Packages with complex
 * customization can now provide custom file roles along with the possibility of
 * adding configuration values to match.
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_Installer_Role_Common
{
    /**
     * @var PEAR_Config
     * @access protected
     */
    var $config;

    /**
     * @param PEAR_Config
     */
    function PEAR_Installer_Role_Common(&$config)
    {
        $this->config = $config;
    }

    /**
     * Retrieve configuration information about a file role from its XML info
     *
     * @param string $role Role Classname, as in "PEAR_Installer_Role_Data"
     * @return array
     */
    function getInfo($role)
    {
        if (empty($GLOBALS['_PEAR_INSTALLER_ROLES'][$role])) {
            return PEAR::raiseError('Unknown Role class: "' . $role . '"');
        }
        return $GLOBALS['_PEAR_INSTALLER_ROLES'][$role];
    }

    /**
     * This is called for each file to set up the directories and files
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @param array attributes from the <file> tag
     * @param string file name
     * @return array an array consisting of:
     *
     *    1 the original, pre-baseinstalldir installation directory
     *    2 the final installation directory
     *    3 the full path to the final location of the file
     *    4 the location of the pre-installation file
     */
    function processInstallation($pkg, $atts, $file, $tmp_path, $layer = null)
    {
        $roleInfo = PEAR_Installer_Role_Common::getInfo('PEAR_Installer_Role_' . 
            ucfirst(str_replace('pear_installer_role_', '', strtolower(get_class($this)))));
        if (PEAR::isError($roleInfo)) {
            return $roleInfo;
        }
        if (!$roleInfo['locationconfig']) {
            return false;
        }
        if ($roleInfo['honorsbaseinstall']) {
            $dest_dir = $save_destdir = $this->config->get($roleInfo['locationconfig'], $layer,
                $pkg->getChannel());
            if (!empty($atts['baseinstalldir'])) {
                $dest_dir .= DIRECTORY_SEPARATOR . $atts['baseinstalldir'];
            }
        } elseif ($roleInfo['unusualbaseinstall']) {
            $dest_dir = $save_destdir = $this->config->get($roleInfo['locationconfig'],
                    null, $pkg->getChannel()) . DIRECTORY_SEPARATOR . $pkg->getPackage();
            if (!empty($atts['baseinstalldir'])) {
                $dest_dir .= DIRECTORY_SEPARATOR . $atts['baseinstalldir'];
            }
        } else {
            $dest_dir = $save_destdir = $this->config->get($roleInfo['locationconfig'],
                    null, $pkg->getChannel()) . DIRECTORY_SEPARATOR . $pkg->getPackage();
        }
        if (dirname($file) != '.' && empty($atts['install-as'])) {
            $dest_dir .= DIRECTORY_SEPARATOR . dirname($file);
        }
        if (empty($atts['install-as'])) {
            $dest_file = $dest_dir . DIRECTORY_SEPARATOR . basename($file);
        } else {
            $dest_file = $dest_dir . DIRECTORY_SEPARATOR . $atts['install-as'];
        }
        $orig_file = $tmp_path . DIRECTORY_SEPARATOR . $file;

        // Clean up the DIRECTORY_SEPARATOR mess
        $ds2 = DIRECTORY_SEPARATOR . DIRECTORY_SEPARATOR;
        
        list($dest_dir, $dest_file, $orig_file) = preg_replace(array('!\\\\+!', '!/!', "!$ds2+!"),
                                                    array(DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR,
                                                          DIRECTORY_SEPARATOR),
                                                    array($dest_dir, $dest_file, $orig_file));
        return array($save_destdir, $dest_dir, $dest_file, $orig_file);
    }

    /**
     * Get the name of the configuration variable that specifies the location of this file
     * @return string|false
     */
    function getLocationConfig()
    {
        $roleInfo = PEAR_Installer_Role_Common::getInfo('PEAR_Installer_Role_' . 
            ucfirst(str_replace('pear_installer_role_', '', strtolower(get_class($this)))));
        if (PEAR::isError($roleInfo)) {
            return $roleInfo;
        }
        return $roleInfo['locationconfig'];
    }

    /**
     * Do any unusual setup here
     * @param PEAR_Installer
     * @param PEAR_PackageFile_v2
     * @param array file attributes
     * @param string file name
     */
    function setup(&$installer, $pkg, $atts, $file)
    {
    }

    function isExecutable()
    {
        $roleInfo = PEAR_Installer_Role_Common::getInfo('PEAR_Installer_Role_' . 
            ucfirst(str_replace('pear_installer_role_', '', strtolower(get_class($this)))));
        if (PEAR::isError($roleInfo)) {
            return $roleInfo;
        }
        return $roleInfo['executable'];
    }

    function isInstallable()
    {
        $roleInfo = PEAR_Installer_Role_Common::getInfo('PEAR_Installer_Role_' . 
            ucfirst(str_replace('pear_installer_role_', '', strtolower(get_class($this)))));
        if (PEAR::isError($roleInfo)) {
            return $roleInfo;
        }
        return $roleInfo['installable'];
    }

    function isExtension()
    {
        $roleInfo = PEAR_Installer_Role_Common::getInfo('PEAR_Installer_Role_' . 
            ucfirst(str_replace('pear_installer_role_', '', strtolower(get_class($this)))));
        if (PEAR::isError($roleInfo)) {
            return $roleInfo;
        }
        return $roleInfo['phpextension'];
    }
}
?>