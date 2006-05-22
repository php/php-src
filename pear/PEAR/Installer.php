<?php
/**
 * PEAR_Installer
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
 * @author     Tomas V.V. Cox <cox@idecnet.com>
 * @author     Martin Jansen <mj@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * Used for installation groups in package.xml 2.0 and platform exceptions
 */
require_once 'OS/Guess.php';
require_once 'PEAR/Downloader.php';

define('PEAR_INSTALLER_NOBINARY', -240);
/**
 * Administration class used to install PEAR packages and maintain the
 * installed package database.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V.V. Cox <cox@idecnet.com>
 * @author     Martin Jansen <mj@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */
class PEAR_Installer extends PEAR_Downloader
{
    // {{{ properties

    /** name of the package directory, for example Foo-1.0
     * @var string
     */
    var $pkgdir;

    /** directory where PHP code files go
     * @var string
     */
    var $phpdir;

    /** directory where PHP extension files go
     * @var string
     */
    var $extdir;

    /** directory where documentation goes
     * @var string
     */
    var $docdir;

    /** installation root directory (ala PHP's INSTALL_ROOT or
     * automake's DESTDIR
     * @var string
     */
    var $installroot = '';

    /** debug level
     * @var int
     */
    var $debug = 1;

    /** temporary directory
     * @var string
     */
    var $tmpdir;

    /**
     * PEAR_Registry object used by the installer
     * @var PEAR_Registry
     */
    var $registry;

    /**
     * array of PEAR_Downloader_Packages
     * @var array
     */
    var $_downloadedPackages;

    /** List of file transactions queued for an install/upgrade/uninstall.
     *
     *  Format:
     *    array(
     *      0 => array("rename => array("from-file", "to-file")),
     *      1 => array("delete" => array("file-to-delete")),
     *      ...
     *    )
     *
     * @var array
     */
    var $file_operations = array();

    // }}}

    // {{{ constructor

    /**
     * PEAR_Installer constructor.
     *
     * @param object $ui user interface object (instance of PEAR_Frontend_*)
     *
     * @access public
     */
    function PEAR_Installer(&$ui)
    {
        parent::PEAR_Common();
        $this->setFrontendObject($ui);
        $this->debug = $this->config->get('verbose');
    }

    function setOptions($options)
    {
        $this->_options = $options;
    }

    function setConfig(&$config)
    {
        $this->config = &$config;
        $this->_registry = &$config->getRegistry();
    }

    // }}}

    function _removeBackups($files)
    {
        foreach ($files as $path) {
            $this->addFileOperation('removebackup', array($path));
        }
    }

    // {{{ _deletePackageFiles()

    /**
     * Delete a package's installed files, does not remove empty directories.
     *
     * @param string package name
     * @param string channel name
     * @param bool if true, then files are backed up first
     * @return bool TRUE on success, or a PEAR error on failure
     * @access protected
     */
    function _deletePackageFiles($package, $channel = false, $backup = false)
    {
        if (!$channel) {
            $channel = 'pear.php.net';
        }
        if (!strlen($package)) {
            return $this->raiseError("No package to uninstall given");
        }
        if (strtolower($package) == 'pear' && $channel == 'pear.php.net') {
            // to avoid race conditions, include all possible needed files
            require_once 'PEAR/Task/Common.php';
            require_once 'PEAR/Task/Replace.php';
            require_once 'PEAR/Task/Unixeol.php';
            require_once 'PEAR/Task/Windowseol.php';
            require_once 'PEAR/PackageFile/v1.php';
            require_once 'PEAR/PackageFile/v2.php';
            require_once 'PEAR/PackageFile/Generator/v1.php';
            require_once 'PEAR/PackageFile/Generator/v2.php';
        }
        $filelist = $this->_registry->packageInfo($package, 'filelist', $channel);
        if ($filelist == null) {
            return $this->raiseError("$channel/$package not installed");
        }
        $ret = array();
        foreach ($filelist as $file => $props) {
            if (empty($props['installed_as'])) {
                continue;
            }
            $path = $props['installed_as'];
            if ($backup) {
                $this->addFileOperation('backup', array($path));
                $ret[] = $path;
            }
            $this->addFileOperation('delete', array($path));
        }
        if ($backup) {
            return $ret;
        }
        return true;
    }

    // }}}
    // {{{ _installFile()

    /**
     * @param string filename
     * @param array attributes from <file> tag in package.xml
     * @param string path to install the file in
     * @param array options from command-line
     * @access private
     */
    function _installFile($file, $atts, $tmp_path, $options)
    {
        // {{{ return if this file is meant for another platform
        static $os;
        if (!isset($this->_registry)) {
            $this->_registry = &$this->config->getRegistry();
        }
        if (isset($atts['platform'])) {
            if (empty($os)) {
                $os = new OS_Guess();
            }
            if (strlen($atts['platform']) && $atts['platform']{0} == '!') {
                $negate = true;
                $platform = substr($atts['platform'], 1);
            } else {
                $negate = false;
                $platform = $atts['platform'];
            }
            if ((bool) $os->matchSignature($platform) === $negate) {
                $this->log(3, "skipped $file (meant for $atts[platform], we are ".$os->getSignature().")");
                return PEAR_INSTALLER_SKIPPED;
            }
        }
        // }}}

        $channel = $this->pkginfo->getChannel();
        // {{{ assemble the destination paths
        switch ($atts['role']) {
            case 'doc':
            case 'data':
            case 'test':
                $dest_dir = $this->config->get($atts['role'] . '_dir', null, $channel) .
                            DIRECTORY_SEPARATOR . $this->pkginfo->getPackage();
                unset($atts['baseinstalldir']);
                break;
            case 'ext':
            case 'php':
                $dest_dir = $this->config->get($atts['role'] . '_dir', null, $channel);
                break;
            case 'script':
                $dest_dir = $this->config->get('bin_dir', null, $channel);
                break;
            case 'src':
            case 'extsrc':
                $this->source_files++;
                return;
            default:
                return $this->raiseError("Invalid role `$atts[role]' for file $file");
        }
        $save_destdir = $dest_dir;
        if (!empty($atts['baseinstalldir'])) {
            $dest_dir .= DIRECTORY_SEPARATOR . $atts['baseinstalldir'];
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
        list($dest_file, $orig_file) = preg_replace(array('!\\\\+!', '!/!', "!$ds2+!"),
                                                    array(DIRECTORY_SEPARATOR,
                                                          DIRECTORY_SEPARATOR,
                                                          DIRECTORY_SEPARATOR),
                                                    array($dest_file, $orig_file));
        $final_dest_file = $installed_as = $dest_file;
        if (isset($this->_options['packagingroot'])) {
            $installedas_dest_dir = dirname($final_dest_file);
            $installedas_dest_file = $dest_dir . DIRECTORY_SEPARATOR . '.tmp' . basename($final_dest_file);
            $final_dest_file = $this->_prependPath($final_dest_file,
                $this->_options['packagingroot']);
        } else {
            $installedas_dest_dir = dirname($final_dest_file);
            $installedas_dest_file = $installedas_dest_dir . DIRECTORY_SEPARATOR . '.tmp' . basename($final_dest_file);
        }
        $dest_dir = dirname($final_dest_file);
        $dest_file = $dest_dir . DIRECTORY_SEPARATOR . '.tmp' . basename($final_dest_file);
        // }}}

        if (empty($this->_options['register-only']) && !@is_dir($dest_dir)) {
            if (!$this->mkDirHier($dest_dir)) {
                return $this->raiseError("failed to mkdir $dest_dir",
                                         PEAR_INSTALLER_FAILED);
            }
            $this->log(3, "+ mkdir $dest_dir");
        }
        // pretty much nothing happens if we are only registering the install
        if (empty($this->_options['register-only'])) {
            if (empty($atts['replacements'])) {
                if (!file_exists($orig_file)) {
                    return $this->raiseError("file $orig_file does not exist",
                                             PEAR_INSTALLER_FAILED);
                }
                if (!@copy($orig_file, $dest_file)) {
                    return $this->raiseError("failed to write $dest_file",
                                             PEAR_INSTALLER_FAILED);
                }
                $this->log(3, "+ cp $orig_file $dest_file");
                if (isset($atts['md5sum'])) {
                    $md5sum = md5_file($dest_file);
                }
            } else {
                // {{{ file with replacements
                if (!file_exists($orig_file)) {
                    return $this->raiseError("file does not exist",
                                             PEAR_INSTALLER_FAILED);
                }
                if (function_exists('file_get_contents')) {
                    $contents = file_get_contents($orig_file);
                } else {
                    $fp = fopen($orig_file, "r");
                    $contents = @fread($fp, filesize($orig_file));
                    fclose($fp);
                }
                if ($contents === false) {
                    $contents = '';
                }
                if (isset($atts['md5sum'])) {
                    $md5sum = md5($contents);
                }
                $subst_from = $subst_to = array();
                foreach ($atts['replacements'] as $a) {
                    $to = '';
                    if ($a['type'] == 'php-const') {
                        if (preg_match('/^[a-z0-9_]+$/i', $a['to'])) {
                            eval("\$to = $a[to];");
                        } else {
                            if (!isset($options['soft'])) {
                                $this->log(0, "invalid php-const replacement: $a[to]");
                            }
                            continue;
                        }
                    } elseif ($a['type'] == 'pear-config') {
                        if ($a['to'] == 'master_server') {
                            $chan = $this->_registry->getChannel($channel);
                            if (!PEAR::isError($chan)) {
                                $to = $chan->getServer();
                            } else {
                                $to = $this->config->get($a['to'], null, $channel);
                            }
                        } else {
                            $to = $this->config->get($a['to'], null, $channel);
                        }
                        if (is_null($to)) {
                            if (!isset($options['soft'])) {
                                $this->log(0, "invalid pear-config replacement: $a[to]");
                            }
                            continue;
                        }
                    } elseif ($a['type'] == 'package-info') {
                        if ($t = $this->pkginfo->packageInfo($a['to'])) {
                            $to = $t;
                        } else {
                            if (!isset($options['soft'])) {
                                $this->log(0, "invalid package-info replacement: $a[to]");
                            }
                            continue;
                        }
                    }
                    if (!is_null($to)) {
                        $subst_from[] = $a['from'];
                        $subst_to[] = $to;
                    }
                }
                $this->log(3, "doing ".sizeof($subst_from)." substitution(s) for $final_dest_file");
                if (sizeof($subst_from)) {
                    $contents = str_replace($subst_from, $subst_to, $contents);
                }
                $wp = @fopen($dest_file, "wb");
                if (!is_resource($wp)) {
                    return $this->raiseError("failed to create $dest_file: $php_errormsg",
                                             PEAR_INSTALLER_FAILED);
                }
                if (fwrite($wp, $contents) === false) {
                    return $this->raiseError("failed writing to $dest_file: $php_errormsg",
                                             PEAR_INSTALLER_FAILED);
                }
                fclose($wp);
                // }}}
            }
            // {{{ check the md5
            if (isset($md5sum)) {
                if (strtolower($md5sum) == strtolower($atts['md5sum'])) {
                    $this->log(2, "md5sum ok: $final_dest_file");
                } else {
                    if (empty($options['force'])) {
                        // delete the file
                        @unlink($dest_file);
                        if (!isset($options['ignore-errors'])) {
                            return $this->raiseError("bad md5sum for file $final_dest_file",
                                                 PEAR_INSTALLER_FAILED);
                        } else {
                            if (!isset($options['soft'])) {
                                $this->log(0, "warning : bad md5sum for file $final_dest_file");
                            }
                        }
                    } else {
                        if (!isset($options['soft'])) {
                            $this->log(0, "warning : bad md5sum for file $final_dest_file");
                        }
                    }
                }
            }
            // }}}
            // {{{ set file permissions
            if (!OS_WINDOWS) {
                if ($atts['role'] == 'script') {
                    $mode = 0777 & ~(int)octdec($this->config->get('umask'));
                    $this->log(3, "+ chmod +x $dest_file");
                } else {
                    $mode = 0666 & ~(int)octdec($this->config->get('umask'));
                }
                $this->addFileOperation("chmod", array($mode, $dest_file));
                if (!@chmod($dest_file, $mode)) {
                    if (!isset($options['soft'])) {
                        $this->log(0, "failed to change mode of $dest_file");
                    }
                }
            }
            // }}}
            $this->addFileOperation("rename", array($dest_file, $final_dest_file,
                $atts['role'] == 'ext'));
        }
        // Store the full path where the file was installed for easy unistall
        $this->addFileOperation("installed_as", array($file, $installed_as,
                                $save_destdir, dirname(substr($installedas_dest_file, strlen($save_destdir)))));

        //$this->log(2, "installed: $dest_file");
        return PEAR_INSTALLER_OK;
    }

    // }}}
    // {{{ _installFile2()

    /**
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     * @param string filename
     * @param array attributes from <file> tag in package.xml
     * @param string path to install the file in
     * @param array options from command-line
     * @access private
     */
    function _installFile2(&$pkg, $file, $atts, $tmp_path, $options)
    {
        if (!isset($this->_registry)) {
            $this->_registry = &$this->config->getRegistry();
        }

        $channel = $pkg->getChannel();
        // {{{ assemble the destination paths
        if (!in_array($atts['attribs']['role'],
              PEAR_Installer_Role::getValidRoles($pkg->getPackageType()))) {
            return $this->raiseError('Invalid role `' . $atts['attribs']['role'] .
                    "' for file $file");
        }
        $role = &PEAR_Installer_Role::factory($pkg, $atts['attribs']['role'], $this->config);
        $err = $role->setup($this, $pkg, $atts['attribs'], $file);
        if (PEAR::isError($err)) {
            return $err;
        }
        if (!$role->isInstallable()) {
            return;
        }
        $info = $role->processInstallation($pkg, $atts['attribs'], $file, $tmp_path);
        if (PEAR::isError($info)) {
            return $info;
        } else {
            list($save_destdir, $dest_dir, $dest_file, $orig_file) = $info;
        }
        $final_dest_file = $installed_as = $dest_file;
        if (isset($this->_options['packagingroot'])) {
            $final_dest_file = $this->_prependPath($final_dest_file,
                $this->_options['packagingroot']);
        }
        $dest_dir = dirname($final_dest_file);
        $dest_file = $dest_dir . DIRECTORY_SEPARATOR . '.tmp' . basename($final_dest_file);
        // }}}

        if (empty($this->_options['register-only'])) {
            if (!@is_dir($dest_dir)) {
                if (!$this->mkDirHier($dest_dir)) {
                    return $this->raiseError("failed to mkdir $dest_dir",
                                             PEAR_INSTALLER_FAILED);
                }
                $this->log(3, "+ mkdir $dest_dir");
            }
        }
        $attribs = $atts['attribs'];
        unset($atts['attribs']);
        // pretty much nothing happens if we are only registering the install
        if (empty($this->_options['register-only'])) {
            if (!count($atts)) { // no tasks
                if (!file_exists($orig_file)) {
                    return $this->raiseError("file $orig_file does not exist",
                                             PEAR_INSTALLER_FAILED);
                }
                if (!@copy($orig_file, $dest_file)) {
                    return $this->raiseError("failed to write $dest_file",
                                             PEAR_INSTALLER_FAILED);
                }
                $this->log(3, "+ cp $orig_file $dest_file");
                if (isset($attribs['md5sum'])) {
                    $md5sum = md5_file($dest_file);
                }
            } else { // file with tasks
                if (!file_exists($orig_file)) {
                    return $this->raiseError("file $orig_file does not exist",
                                             PEAR_INSTALLER_FAILED);
                }
                if (function_exists('file_get_contents')) {
                    $contents = file_get_contents($orig_file);
                } else {
                    $fp = fopen($orig_file, "r");
                    $contents = @fread($fp, filesize($orig_file)); // filesize can be 0
                    fclose($fp);
                }
                if ($contents === false) {
                    $contents = '';
                }
                if (isset($attribs['md5sum'])) {
                    $md5sum = md5($contents);
                }
                foreach ($atts as $tag => $raw) {
                    $tag = str_replace($pkg->getTasksNs() . ':', '', $tag);
                    $task = "PEAR_Task_$tag";
                    $task = &new $task($this->config, $this, PEAR_TASK_INSTALL);
                    if (!$task->isScript()) { // scripts are only handled after installation
                        $task->init($raw, $attribs, $pkg->getLastInstalledVersion());
                        $res = $task->startSession($pkg, $contents, $final_dest_file);
                        if ($res === false) {
                            continue; // skip this file
                        }
                        if (PEAR::isError($res)) {
                            return $res;
                        }
                        $contents = $res; // save changes
                    }
                    $wp = @fopen($dest_file, "wb");
                    if (!is_resource($wp)) {
                        return $this->raiseError("failed to create $dest_file: $php_errormsg",
                                                 PEAR_INSTALLER_FAILED);
                    }
                    if (fwrite($wp, $contents) === false) {
                        return $this->raiseError("failed writing to $dest_file: $php_errormsg",
                                                 PEAR_INSTALLER_FAILED);
                    }
                    fclose($wp);
                }
            }
            // {{{ check the md5
            if (isset($md5sum)) {
                if (strtolower($md5sum) == strtolower($attribs['md5sum'])) {
                    $this->log(2, "md5sum ok: $final_dest_file");
                } else {
                    if (empty($options['force'])) {
                        // delete the file
                        @unlink($dest_file);
                        if (!isset($options['ignore-errors'])) {
                            return $this->raiseError("bad md5sum for file $final_dest_file",
                                                     PEAR_INSTALLER_FAILED);
                        } else {
                            if (!isset($options['soft'])) {
                                $this->log(0, "warning : bad md5sum for file $final_dest_file");
                            }
                        }
                    } else {
                        if (!isset($options['soft'])) {
                            $this->log(0, "warning : bad md5sum for file $final_dest_file");
                        }
                    }
                }
            }
            // }}}
            // {{{ set file permissions
            if (!OS_WINDOWS) {
                if ($role->isExecutable()) {
                    $mode = 0777 & ~(int)octdec($this->config->get('umask'));
                    $this->log(3, "+ chmod +x $dest_file");
                } else {
                    $mode = 0666 & ~(int)octdec($this->config->get('umask'));
                }
                $this->addFileOperation("chmod", array($mode, $dest_file));
                if (!@chmod($dest_file, $mode)) {
                    if (!isset($options['soft'])) {
                        $this->log(0, "failed to change mode of $dest_file");
                    }
                }
            }
            // }}}
            $this->addFileOperation("rename", array($dest_file, $final_dest_file, $role->isExtension()));
        }
        // Store the full path where the file was installed for easy uninstall
        $this->addFileOperation("installed_as", array($file, $installed_as,
                            $save_destdir, dirname(substr($dest_file, strlen($save_destdir)))));

        //$this->log(2, "installed: $dest_file");
        return PEAR_INSTALLER_OK;
    }

    // }}}
    // {{{ addFileOperation()

    /**
     * Add a file operation to the current file transaction.
     *
     * @see startFileTransaction()
     * @param string $type This can be one of:
     *    - rename:  rename a file ($data has 3 values)
     *    - backup:  backup an existing file ($data has 1 value)
     *    - removebackup:  clean up backups created during install ($data has 1 value)
     *    - chmod:   change permissions on a file ($data has 2 values)
     *    - delete:  delete a file ($data has 1 value)
     *    - rmdir:   delete a directory if empty ($data has 1 value)
     *    - installed_as: mark a file as installed ($data has 4 values).
     * @param array $data For all file operations, this array must contain the
     *    full path to the file or directory that is being operated on.  For
     *    the rename command, the first parameter must be the file to rename,
     *    the second its new name, the third whether this is a PHP extension.
     *
     *    The installed_as operation contains 4 elements in this order:
     *    1. Filename as listed in the filelist element from package.xml
     *    2. Full path to the installed file
     *    3. Full path from the php_dir configuration variable used in this
     *       installation
     *    4. Relative path from the php_dir that this file is installed in
     */
    function addFileOperation($type, $data)
    {
        if (!is_array($data)) {
            return $this->raiseError('Internal Error: $data in addFileOperation'
                . ' must be an array, was ' . gettype($data));
        }
        if ($type == 'chmod') {
            $octmode = decoct($data[0]);
            $this->log(3, "adding to transaction: $type $octmode $data[1]");
        } else {
            $this->log(3, "adding to transaction: $type " . implode(" ", $data));
        }
        $this->file_operations[] = array($type, $data);
    }

    // }}}
    // {{{ startFileTransaction()

    function startFileTransaction($rollback_in_case = false)
    {
        if (count($this->file_operations) && $rollback_in_case) {
            $this->rollbackFileTransaction();
        }
        $this->file_operations = array();
    }

    // }}}
    // {{{ commitFileTransaction()

    function commitFileTransaction()
    {
        $n = count($this->file_operations);
        $this->log(2, "about to commit $n file operations");
        // {{{ first, check permissions and such manually
        $errors = array();
        foreach ($this->file_operations as $tr) {
            list($type, $data) = $tr;
            switch ($type) {
                case 'rename':
                    if (!file_exists($data[0])) {
                        $errors[] = "cannot rename file $data[0], doesn't exist";
                    }
                    // check that dest dir. is writable
                    if (!is_writable(dirname($data[1]))) {
                        $errors[] = "permission denied ($type): $data[1]";
                    }
                    break;
                case 'chmod':
                    // check that file is writable
                    if (!is_writable($data[1])) {
                        $errors[] = "permission denied ($type): $data[1] " . decoct($data[0]);
                    }
                    break;
                case 'delete':
                    if (!file_exists($data[0])) {
                        $this->log(2, "warning: file $data[0] doesn't exist, can't be deleted");
                    }
                    // check that directory is writable
                    if (file_exists($data[0])) {
                        if (!is_writable(dirname($data[0]))) {
                            $errors[] = "permission denied ($type): $data[0]";
                        } else {
                            // make sure the file to be deleted can be opened for writing
                            $fp = false;
                            if (!is_dir($data[0]) && !($fp = @fopen($data[0], 'a'))) {
                                $errors[] = "permission denied ($type): $data[0]";
                            } elseif ($fp) {
                                fclose($fp);
                            }
                        }
                    }
                    break;
            }

        }
        // }}}
        $m = sizeof($errors);
        if ($m > 0) {
            foreach ($errors as $error) {
                if (!isset($this->_options['soft'])) {
                    $this->log(1, $error);
                }
            }
            if (!isset($this->_options['ignore-errors'])) {
                return false;
            }
        }
        $this->_dirtree = array();
        // {{{ really commit the transaction
        foreach ($this->file_operations as $tr) {
            list($type, $data) = $tr;
            switch ($type) {
                case 'backup':
                    @copy($data[0], $data[0] . '.bak');
                    $this->log(3, "+ backup $data[0] to $data[0].bak");
                    break;
                case 'removebackup':
                    @unlink($data[0] . '.bak');
                    $this->log(3, "+ rm backup of $data[0] ($data[0].bak)");
                    break;
                case 'rename':
                    $test = @unlink($data[1]);
                    if (!$test && file_exists($data[1])) {
                        if ($data[2]) {
                            $extra = ', this extension must be installed manually.  Rename to "' .
                                basename($data[1]) . '"';
                        } else {
                            $extra = '';
                        }
                        if (!isset($this->_options['soft'])) {
                            $this->log(1, 'Could not delete ' . $data[1] . ', cannot rename ' .
                                $data[0] . $extra);
                        }
                        if (!isset($this->_options['ignore-errors'])) {
                            return false;
                        }
                    }
                    @rename($data[0], $data[1]);
                    $this->log(3, "+ mv $data[0] $data[1]");
                    break;
                case 'chmod':
                    @chmod($data[1], $data[0]);
                    $octmode = decoct($data[0]);
                    $this->log(3, "+ chmod $octmode $data[1]");
                    break;
                case 'delete':
                    @unlink($data[0]);
                    $this->log(3, "+ rm $data[0]");
                    break;
                case 'rmdir':
                    @rmdir($data[0]);
                    $this->log(3, "+ rmdir $data[0]");
                    break;
                case 'installed_as':
                    $this->pkginfo->setInstalledAs($data[0], $data[1]);
                    if (!isset($this->_dirtree[dirname($data[1])])) {
                        $this->_dirtree[dirname($data[1])] = true;
                        $this->pkginfo->setDirtree(dirname($data[1]));

                        while(!empty($data[3]) && $data[3] != '/' && $data[3] != '\\'
                              && $data[3] != '.') {
                            $this->pkginfo->setDirtree($pp =
                                $this->_prependPath($data[3], $data[2]));
                            $this->_dirtree[$pp] = true;
                            $data[3] = dirname($data[3]);
                        }
                    }
                    break;
            }
        }
        // }}}
        $this->log(2, "successfully committed $n file operations");
        $this->file_operations = array();
        return true;
    }

    // }}}
    // {{{ rollbackFileTransaction()

    function rollbackFileTransaction()
    {
        $n = count($this->file_operations);
        $this->log(2, "rolling back $n file operations");
        foreach ($this->file_operations as $tr) {
            list($type, $data) = $tr;
            switch ($type) {
                case 'backup':
                    if (file_exists($data[0] . '.bak')) {
                        @unlink($data[0]);
                        @copy($data[0] . '.bak', $data[0]);
                        $this->log(3, "+ restore $data[0] from $data[0].bak");
                    }
                    break;
                case 'rename':
                    @unlink($data[0]);
                    $this->log(3, "+ rm $data[0]");
                    break;
                case 'mkdir':
                    @rmdir($data[0]);
                    $this->log(3, "+ rmdir $data[0]");
                    break;
                case 'chmod':
                    break;
                case 'delete':
                    break;
                case 'installed_as':
                    $this->pkginfo->setInstalledAs($data[0], false);
                    break;
            }
        }
        $this->pkginfo->resetDirtree();
        $this->file_operations = array();
    }

    // }}}
    // {{{ mkDirHier($dir)

    function mkDirHier($dir)
    {
        $this->addFileOperation('mkdir', array($dir));
        return parent::mkDirHier($dir);
    }

    // }}}
    // {{{ download()

    /**
     * Download any files and their dependencies, if necessary
     *
     * @param array a mixed list of package names, local files, or package.xml
     * @param PEAR_Config
     * @param array options from the command line
     * @param array this is the array that will be populated with packages to
     *              install.  Format of each entry:
     *
     * <code>
     * array('pkg' => 'package_name', 'file' => '/path/to/local/file',
     *    'info' => array() // parsed package.xml
     * );
     * </code>
     * @param array this will be populated with any error messages
     * @param false private recursion variable
     * @param false private recursion variable
     * @param false private recursion variable
     * @deprecated in favor of PEAR_Downloader
     */
    function download($packages, $options, &$config, &$installpackages,
                      &$errors, $installed = false, $willinstall = false, $state = false)
    {
        // trickiness: initialize here
        parent::PEAR_Downloader($this->ui, $options, $config);
        $ret = parent::download($packages);
        $errors = $this->getErrorMsgs();
        $installpackages = $this->getDownloadedPackages();
        trigger_error("PEAR Warning: PEAR_Installer::download() is deprecated " .
                      "in favor of PEAR_Downloader class", E_USER_WARNING);
        return $ret;
    }

    // }}}
    // {{{ _parsePackageXml()

    function _parsePackageXml(&$descfile, &$tmpdir)
    {
        if (substr($descfile, -4) == '.xml') {
            $tmpdir = false;
        } else {
            // {{{ Decompress pack in tmp dir -------------------------------------

            // To allow relative package file names
            $descfile = realpath($descfile);

            if (PEAR::isError($tmpdir = System::mktemp('-d'))) {
                return $tmpdir;
            }
            $this->log(3, '+ tmp dir created at ' . $tmpdir);
            // }}}
        }
        // Parse xml file -----------------------------------------------
        $pkg = new PEAR_PackageFile($this->config, $this->debug, $tmpdir);
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $p = &$pkg->fromAnyFile($descfile, PEAR_VALIDATE_INSTALLING);
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($p)) {
            if (is_array($p->getUserInfo())) {
                foreach ($p->getUserInfo() as $err) {
                    $loglevel = $err['level'] == 'error' ? 0 : 1;
                    if (!isset($this->_options['soft'])) {
                        $this->log($loglevel, ucfirst($err['level']) . ': ' . $err['message']);
                    }
                }
            }
            return $this->raiseError('Installation failed: invalid package file');
        } else {
            $descfile = $p->getPackageFile();
        }
        return $p;
    }

    // }}}
    /**
     * Set the list of PEAR_Downloader_Package objects to allow more sane
     * dependency validation
     * @param array
     */
    function setDownloadedPackages(&$pkgs)
    {
        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
        $err = $this->analyzeDependencies($pkgs);
        PEAR::popErrorHandling();
        if (PEAR::isError($err)) {
            return $err;
        }
        $this->_downloadedPackages = &$pkgs;
    }

    /**
     * Set the list of PEAR_Downloader_Package objects to allow more sane
     * dependency validation
     * @param array
     */
    function setUninstallPackages(&$pkgs)
    {
        $this->_downloadedPackages = &$pkgs;
    }

    function getInstallPackages()
    {
        return $this->_downloadedPackages;
    }

    // {{{ install()

    /**
     * Installs the files within the package file specified.
     *
     * @param string|PEAR_Downloader_Package $pkgfile path to the package file,
     *        or a pre-initialized packagefile object
     * @param array $options
     * recognized options:
     * - installroot   : optional prefix directory for installation
     * - force         : force installation
     * - register-only : update registry but don't install files
     * - upgrade       : upgrade existing install
     * - soft          : fail silently
     * - nodeps        : ignore dependency conflicts/missing dependencies
     * - alldeps       : install all dependencies
     * - onlyreqdeps   : install only required dependencies
     *
     * @return array|PEAR_Error package info if successful
     */

    function install($pkgfile, $options = array())
    {
        $this->_options = $options;
        $this->_registry = &$this->config->getRegistry();
        if (is_object($pkgfile)) {
            $dlpkg = &$pkgfile;
            $pkg = $pkgfile->getPackageFile();
            $pkgfile = $pkg->getArchiveFile();
            $descfile = $pkg->getPackageFile();
            $tmpdir = dirname($descfile);
        } else {
            $descfile = $pkgfile;
            $tmpdir = '';
            if (PEAR::isError($pkg = &$this->_parsePackageXml($descfile, $tmpdir))) {
                return $pkg;
            }
        }

        if (realpath($descfile) != realpath($pkgfile)) {
            $tar = new Archive_Tar($pkgfile);
            if (!@$tar->extract($tmpdir)) {
                return $this->raiseError("unable to unpack $pkgfile");
            }
        }

        $pkgname = $pkg->getName();
        $channel = $pkg->getChannel();
        if (isset($this->_options['packagingroot'])) {
            $packrootphp_dir = $this->_prependPath(
                $this->config->get('php_dir', null, $channel),
                $this->_options['packagingroot']);
        }

        if (isset($options['installroot'])) {
            $this->config->setInstallRoot($options['installroot']);
            $this->_registry = &$this->config->getRegistry();
            $installregistry = &$this->_registry;
            $this->installroot = ''; // all done automagically now
            $php_dir = $this->config->get('php_dir', null, $channel);
        } else {
            $this->config->setInstallRoot(false);
            $this->_registry = &$this->config->getRegistry();
            if (isset($this->_options['packagingroot'])) {
                $installregistry = &new PEAR_Registry($packrootphp_dir);
                $php_dir = $packrootphp_dir;
            } else {
                $installregistry = &$this->_registry;
                $php_dir = $this->config->get('php_dir', null, $channel);
            }
            $this->installroot = '';
        }

        // {{{ checks to do when not in "force" mode
        if (empty($options['force']) && @is_dir($this->config->get('php_dir'))) {
            $testp = $channel == 'pear.php.net' ? $pkgname : array($channel, $pkgname);
            $instfilelist = $pkg->getInstallationFileList(true);
            if (PEAR::isError($instfilelist)) {
                return $instfilelist;
            }
            $test = $installregistry->checkFileMap($instfilelist, $testp, '1.1');
            if (PEAR::isError($test)) {
                return $test;
            }
            if (sizeof($test)) {
                $pkgs = $this->getInstallPackages();
                $found = false;
                foreach ($pkgs as $param) {
                    if ($pkg->isSubpackageOf($param)) {
                        $found = true;
                        break;
                    }
                }
                if ($found) {
                    // subpackages can conflict with earlier versions of parent packages
                    $parentreg = $installregistry->packageInfo($param->getPackage(), null, $param->getChannel());
                    $tmp = $test;
                    foreach ($tmp as $file => $info) {
                        if (is_array($info)) {
                            if (strtolower($info[1]) == strtolower($param->getPackage()) &&
                                  strtolower($info[0]) == strtolower($param->getChannel())) {
                                unset($test[$file]);
                                unset($parentreg['filelist'][$file]);
                            }
                        } else {
                            if (strtolower($param->getChannel()) != 'pear.php.net') {
                                continue;
                            }
                            if (strtolower($info) == strtolower($param->getPackage())) {
                                unset($test[$file]);
                                unset($parentreg['filelist'][$file]);
                            }
                        }
                    }
                    $pfk = &new PEAR_PackageFile($this->config);
                    $parentpkg = &$pfk->fromArray($parentreg);
                    $installregistry->updatePackage2($parentpkg);
                }
                if ($param->getChannel() == 'pecl.php.net' && isset($options['upgrade'])) {
                    $tmp = $test;
                    foreach ($tmp as $file => $info) {
                        if (is_string($info)) {
                            // pear.php.net packages are always stored as strings
                            if (strtolower($info) == strtolower($param->getPackage())) {
                                // upgrading existing package
                                unset($test[$file]);
                            }
                        }
                    }
                }
                if (sizeof($test)) {
                    $msg = "$channel/$pkgname: conflicting files found:\n";
                    $longest = max(array_map("strlen", array_keys($test)));
                    $fmt = "%${longest}s (%s)\n";
                    foreach ($test as $file => $info) {
                        if (!is_array($info)) {
                            $info = array('pear.php.net', $info);
                        }
                        $info = $info[0] . '/' . $info[1];
                        $msg .= sprintf($fmt, $file, $info);
                    }
                    if (!isset($options['ignore-errors'])) {
                        return $this->raiseError($msg);
                    } else {
                        if (!isset($options['soft'])) {
                            $this->log(0, "WARNING: $msg");
                        }
                    }
                }
            }
        }
        // }}}

        $this->startFileTransaction();

        if (empty($options['upgrade']) && empty($options['soft'])) {
            // checks to do only when installing new packages
            if ($channel == 'pecl.php.net') {
                $test = $installregistry->packageExists($pkgname, $channel);
                if (!$test) {
                    $test = $installregistry->packageExists($pkgname, 'pear.php.net');
                }
            } else {
                $test = $installregistry->packageExists($pkgname, $channel);
            }
            if (empty($options['force']) && $test) {
                return $this->raiseError("$channel/$pkgname is already installed");
            }
        } else {
            $usechannel = $channel;
            if ($channel == 'pecl.php.net') {
                $test = $installregistry->packageExists($pkgname, $channel);
                if (!$test) {
                    $test = $installregistry->packageExists($pkgname, 'pear.php.net');
                    $usechannel = 'pear.php.net';
                }
            } else {
                $test = $installregistry->packageExists($pkgname, $channel);
            }
            if ($test) {
                $v1 = $installregistry->packageInfo($pkgname, 'version', $usechannel);
                $v2 = $pkg->getVersion();
                $cmp = version_compare("$v1", "$v2", 'gt');
                if (empty($options['force']) && !version_compare("$v2", "$v1", 'gt')) {
                    return $this->raiseError("upgrade to a newer version ($v2 is not newer than $v1)");
                }
                if (empty($options['register-only'])) {
                    // when upgrading, remove old release's files first:
                    if (PEAR::isError($err = $this->_deletePackageFiles($pkgname, $usechannel,
                          true))) {
                        if (!isset($options['ignore-errors'])) {
                            return $this->raiseError($err);
                        } else {
                            if (!isset($options['soft'])) {
                                $this->log(0, 'WARNING: ' . $err->getMessage());
                            }
                        }
                    } else {
                        $backedup = $err;
                    }
                }
            }
        }

        // {{{ Copy files to dest dir ---------------------------------------

        // info from the package it self we want to access from _installFile
        $this->pkginfo = &$pkg;
        // used to determine whether we should build any C code
        $this->source_files = 0;

        $savechannel = $this->config->get('default_channel');
        if (empty($options['register-only']) && !is_dir($php_dir)) {
            if (PEAR::isError(System::mkdir(array('-p'), $php_dir))) {
                return $this->raiseError("no installation destination directory '$php_dir'\n");
            }
        }

        $tmp_path = dirname($descfile);
        if (substr($pkgfile, -4) != '.xml') {
            $tmp_path .= DIRECTORY_SEPARATOR . $pkgname . '-' . $pkg->getVersion();
        }

        $this->configSet('default_channel', $channel);
        // {{{ install files
        
        if ($pkg->getPackagexmlVersion() == '2.0') {
            $filelist = $pkg->getInstallationFilelist();
        } else {
            $filelist = $pkg->getFileList();
        }
        if (PEAR::isError($filelist)) {
            return $filelist;
        }
        $pkg->resetFilelist();
        $pkg->setLastInstalledVersion($installregistry->packageInfo($pkg->getPackage(),
            'version', $pkg->getChannel()));
        foreach ($filelist as $file => $atts) {
            if ($pkg->getPackagexmlVersion() == '1.0') {
                $this->expectError(PEAR_INSTALLER_FAILED);
                $res = $this->_installFile($file, $atts, $tmp_path, $options);
                $this->popExpect();
            } else {
                $this->expectError(PEAR_INSTALLER_FAILED);
                $res = $this->_installFile2($pkg, $file, $atts, $tmp_path, $options);
                $this->popExpect();
            }
            if (PEAR::isError($res)) {
                if (empty($options['ignore-errors'])) {
                    $this->rollbackFileTransaction();
                    if ($res->getMessage() == "file does not exist") {
                        $this->raiseError("file $file in package.xml does not exist");
                    }
                    return $this->raiseError($res);
                } else {
                    if (!isset($options['soft'])) {
                        $this->log(0, "Warning: " . $res->getMessage());
                    }
                }
            }
            if ($res == PEAR_INSTALLER_OK) {
                // Register files that were installed
                $pkg->installedFile($file, $atts);
            }
        }
        // }}}

        // {{{ compile and install source files
        if ($this->source_files > 0 && empty($options['nobuild'])) {
            if (PEAR::isError($err =
                  $this->_compileSourceFiles($savechannel, $pkg))) {
                return $err;
            }
        }
        // }}}

        if (isset($backedup)) {
            $this->_removeBackups($backedup);
        }
        if (!$this->commitFileTransaction()) {
            $this->rollbackFileTransaction();
            $this->configSet('default_channel', $savechannel);
            return $this->raiseError("commit failed", PEAR_INSTALLER_FAILED);
        }
        // }}}

        $ret = false;
        $installphase = 'install';
        $oldversion = false;
        // {{{ Register that the package is installed -----------------------
        if (empty($options['upgrade'])) {
            // if 'force' is used, replace the info in registry
            $usechannel = $channel;
            if ($channel == 'pecl.php.net') {
                $test = $installregistry->packageExists($pkgname, $channel);
                if (!$test) {
                    $test = $installregistry->packageExists($pkgname, 'pear.php.net');
                    $usechannel = 'pear.php.net';
                }
            } else {
                $test = $installregistry->packageExists($pkgname, $channel);
            }
            if (!empty($options['force']) && $test) {
                $oldversion = $installregistry->packageInfo($pkgname, 'version', $usechannel);
                $installregistry->deletePackage($pkgname, $usechannel);
            }
            $ret = $installregistry->addPackage2($pkg);
        } else {
            $usechannel = $channel;
            if ($channel == 'pecl.php.net') {
                $test = $installregistry->packageExists($pkgname, $channel);
                if (!$test) {
                    $test = $installregistry->packageExists($pkgname, 'pear.php.net');
                    $usechannel = 'pear.php.net';
                }
            } else {
                $test = $installregistry->packageExists($pkgname, $channel);
            }
            // new: upgrade installs a package if it isn't installed
            if (!$test) {
                $ret = $installregistry->addPackage2($pkg);
            } else {
                if ($usechannel != $channel) {
                    $installregistry->deletePackage($pkgname, $usechannel);
                    $ret = $installregistry->addPackage2($pkg);
                } else {
                    $ret = $installregistry->updatePackage2($pkg);
                }
                $installphase = 'upgrade';
            }
        }
        if (!$ret) {
            $this->configSet('default_channel', $savechannel);
            return $this->raiseError("Adding package $channel/$pkgname to registry failed");
        }
        // }}}
        $this->configSet('default_channel', $savechannel);
        if (class_exists('PEAR_Task_Common')) { // this is auto-included if any tasks exist
            if (PEAR_Task_Common::hasPostinstallTasks()) {
                PEAR_Task_Common::runPostinstallTasks($installphase);
            }
        }
        return $pkg->toArray(true);
    }

    // }}}

    // {{{ _compileSourceFiles()
    /**
     * @param string
     * @param PEAR_PackageFile_v1|PEAR_PackageFile_v2
     */
    function _compileSourceFiles($savechannel, &$filelist)
    {
        require_once 'PEAR/Builder.php';
        $this->log(1, "$this->source_files source files, building");
        $bob = &new PEAR_Builder($this->ui);
        $bob->debug = $this->debug;
        $built = $bob->build($filelist, array(&$this, '_buildCallback'));
        if (PEAR::isError($built)) {
            $this->rollbackFileTransaction();
            $this->configSet('default_channel', $savechannel);
            return $built;
        }
        $this->log(1, "\nBuild process completed successfully");
        foreach ($built as $ext) {
            $bn = basename($ext['file']);
            list($_ext_name, $_ext_suff) = explode('.', $bn);
            if ($_ext_suff == '.so' || $_ext_suff == '.dll') {
                if (extension_loaded($_ext_name)) {
                    $this->raiseError("Extension '$_ext_name' already loaded. " .
                                      'Please unload it in your php.ini file ' .
                                      'prior to install or upgrade');
                }
                $role = 'ext';
            } else {
                $role = 'src';
            }
            $dest = $ext['dest'];
            $this->log(1, "Installing '$ext[file]'");
            $packagingroot = '';
            if (isset($this->_options['packagingroot'])) {
                $packagingroot = $this->_options['packagingroot'];
            }
            $copyto = $this->_prependPath($dest, $packagingroot);
            $copydir = dirname($copyto);
            // pretty much nothing happens if we are only registering the install
            if (empty($this->_options['register-only'])) {
                if (!@is_dir($copydir)) {
                    if (!$this->mkDirHier($copydir)) {
                        return $this->raiseError("failed to mkdir $copydir",
                            PEAR_INSTALLER_FAILED);
                    }
                    $this->log(3, "+ mkdir $copydir");
                }
                if (!@copy($ext['file'], $copyto)) {
                    return $this->raiseError("failed to write $copyto", PEAR_INSTALLER_FAILED);
                }
                $this->log(3, "+ cp $ext[file] $copyto");
                if (!OS_WINDOWS) {
                    $mode = 0666 & ~(int)octdec($this->config->get('umask'));
                    $this->addFileOperation('chmod', array($mode, $copyto));
                    if (!@chmod($copyto, $mode)) {
                        $this->log(0, "failed to change mode of $copyto");
                    }
                }
                $this->addFileOperation('rename', array($ext['file'], $copyto));
            }

            if ($filelist->getPackageXmlVersion() == '1.0') {
                $filelist->installedFile($bn, array(
                    'role' => $role,
                    'name' => $bn,
                    'installed_as' => $dest,
                    'php_api' => $ext['php_api'],
                    'zend_mod_api' => $ext['zend_mod_api'],
                    'zend_ext_api' => $ext['zend_ext_api'],
                    ));
            } else {
                $filelist->installedFile($bn, array('attribs' => array(
                    'role' => $role,
                    'name' => $bn,
                    'installed_as' => $dest,
                    'php_api' => $ext['php_api'],
                    'zend_mod_api' => $ext['zend_mod_api'],
                    'zend_ext_api' => $ext['zend_ext_api'],
                    )));
            }
        }
    }

    // }}}
    function &getUninstallPackages()
    {
        return $this->_downloadedPackages;
    }
    // {{{ uninstall()

    /**
     * Uninstall a package
     *
     * This method removes all files installed by the application, and then
     * removes any empty directories.
     * @param string package name
     * @param array Command-line options.  Possibilities include:
     *
     *              - installroot: base installation dir, if not the default
     *              - nodeps: do not process dependencies of other packages to ensure
     *                        uninstallation does not break things
     */
    function uninstall($package, $options = array())
    {
        if (isset($options['installroot'])) {
            $this->config->setInstallRoot($options['installroot']);
            $this->installroot = '';
        } else {
            $this->config->setInstallRoot('');
            $this->installroot = '';
        }
        $this->_registry = &$this->config->getRegistry();
        if (is_object($package)) {
            $channel = $package->getChannel();
            $pkg = $package;
            $package = $pkg->getPackage();
        } else {
            $pkg = false;
            $info = $this->_registry->parsePackageName($package,
                $this->config->get('default_channel'));
            $channel = $info['channel'];
            $package = $info['package'];
        }
        $savechannel = $this->config->get('default_channel');
        $this->configSet('default_channel', $channel);
        if (!is_object($pkg)) {
            $pkg = $this->_registry->getPackage($package, $channel);
        }
        if (!$pkg) {
            $this->configSet('default_channel', $savechannel);
            return $this->raiseError($this->_registry->parsedPackageNameToString(
                array(
                    'channel' => $channel,
                    'package' => $package
                ), true) . ' not installed');
        }
        if ($pkg->getInstalledBinary()) {
            // this is just an alias for a binary package
            return $this->_registry->deletePackage($package, $channel);
        }
        $filelist = $pkg->getFilelist();
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        if (!class_exists('PEAR_Dependency2')) {
            require_once 'PEAR/Dependency2.php';
        }
        $depchecker = &new PEAR_Dependency2($this->config, $options, 
            array('channel' => $channel, 'package' => $package),
            PEAR_VALIDATE_UNINSTALLING);
        $e = $depchecker->validatePackageUninstall($this);
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($e)) {
            if (!isset($options['ignore-errors'])) {
                return $this->raiseError($e);
            } else {
                if (!isset($options['soft'])) {
                    $this->log(0, 'WARNING: ' . $e->getMessage());
                }
            }
        } elseif (is_array($e)) {
            if (!isset($options['soft'])) {
                $this->log(0, $e[0]);
            }
        }
        // {{{ Delete the files
        $this->startFileTransaction();
        PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
        if (PEAR::isError($err = $this->_deletePackageFiles($package, $channel))) {
            PEAR::popErrorHandling();
            $this->rollbackFileTransaction();
            $this->configSet('default_channel', $savechannel);
            if (!isset($options['ignore-errors'])) {
                return $this->raiseError($err);
            } else {
                if (!isset($options['soft'])) {
                    $this->log(0, 'WARNING: ' . $err->getMessage());
                }
            }
        } else {
            PEAR::popErrorHandling();
        }
        if (!$this->commitFileTransaction()) {
            $this->rollbackFileTransaction();
            if (!isset($options['ignore-errors'])) {
                return $this->raiseError("uninstall failed");
            } elseif (!isset($options['soft'])) {
                $this->log(0, 'WARNING: uninstall failed');
            }
        } else {
            $this->startFileTransaction();
            if ($dirtree = $pkg->getDirTree()) {
                // attempt to delete empty directories
                uksort($dirtree, array($this, '_sortDirs'));
                foreach($dirtree as $dir => $notused) {
                    $this->addFileOperation('rmdir', array($dir));
                }
            } else {
                $this->configSet('default_channel', $savechannel);
                return $this->_registry->deletePackage($package, $channel);
            }
            if (!$this->commitFileTransaction()) {
                $this->rollbackFileTransaction();
            }
        }
        // }}}

        $this->configSet('default_channel', $savechannel);
        // Register that the package is no longer installed
        return $this->_registry->deletePackage($package, $channel);
    }

    /**
     * Sort a list of arrays of array(downloaded packagefilename) by dependency.
     *
     * It also removes duplicate dependencies
     * @param array an array of PEAR_PackageFile_v[1/2] objects
     * @return array|PEAR_Error array of array(packagefilename, package.xml contents)
     */
    function sortPackagesForUninstall(&$packages)
    {
        $this->_dependencyDB = &PEAR_DependencyDB::singleton($this->config);
        if (PEAR::isError($this->_dependencyDB)) {
            return $this->_dependencyDB;
        }
        usort($packages, array(&$this, '_sortUninstall'));
    }

    function _sortUninstall($a, $b)
    {
        if (!$a->getDeps() && !$b->getDeps()) {
            return 0; // neither package has dependencies, order is insignificant
        }
        if ($a->getDeps() && !$b->getDeps()) {
            return -1; // $a must be installed after $b because $a has dependencies
        }
        if (!$a->getDeps() && $b->getDeps()) {
            return 1; // $b must be installed after $a because $b has dependencies
        }
        // both packages have dependencies
        if ($this->_dependencyDB->dependsOn($a, $b)) {
            return -1;
        }
        if ($this->_dependencyDB->dependsOn($b, $a)) {
            return 1;
        }
        return 0;
    }

    // }}}
    // {{{ _sortDirs()
    function _sortDirs($a, $b)
    {
        if (strnatcmp($a, $b) == -1) return 1;
        if (strnatcmp($a, $b) == 1) return -1;
        return 0;
    }

    // }}}

    // {{{ _buildCallback()

    function _buildCallback($what, $data)
    {
        if (($what == 'cmdoutput' && $this->debug > 1) ||
            ($what == 'output' && $this->debug > 0)) {
            $this->ui->outputData(rtrim($data), 'build');
        }
    }

    // }}}
}

// {{{ md5_file() utility function
if (!function_exists("md5_file")) {
    function md5_file($filename) {
        $fp = fopen($filename, "r");
        if (!$fp) return null;
        if (function_exists('file_get_contents')) {
            fclose($fp);
            $contents = file_get_contents($filename);
        } else {
            $contents = fread($fp, filesize($filename));
            fclose($fp);
        }
        return md5($contents);
    }
}
// }}}

?>