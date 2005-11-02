<?php
/**
 * PEAR_Command_Package (package, package-validate, cvsdiff, cvstag, package-dependencies,
 * sign, makerpm, convert commands)
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
 * @author     Martin Jansen <mj@php.net>
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
require_once 'PEAR/Command/Common.php';

/**
 * PEAR commands for login/logout
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Martin Jansen <mj@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */

class PEAR_Command_Package extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'package' => array(
            'summary' => 'Build Package',
            'function' => 'doPackage',
            'shortcut' => 'p',
            'options' => array(
                'nocompress' => array(
                    'shortopt' => 'Z',
                    'doc' => 'Do not gzip the package file'
                    ),
                'showname' => array(
                    'shortopt' => 'n',
                    'doc' => 'Print the name of the packaged file.',
                    ),
                ),
            'doc' => '[descfile] [descfile2]
Creates a PEAR package from its description file (usually called
package.xml).  If a second packagefile is passed in, then
the packager will check to make sure that one is a package.xml
version 1.0, and the other is a package.xml version 2.0.  The
package.xml version 1.0 will be saved as "package.xml" in the archive,
and the other as "package2.xml" in the archive"
'
            ),
        'package-validate' => array(
            'summary' => 'Validate Package Consistency',
            'function' => 'doPackageValidate',
            'shortcut' => 'pv',
            'options' => array(),
            'doc' => '
',
            ),
        'cvsdiff' => array(
            'summary' => 'Run a "cvs diff" for all files in a package',
            'function' => 'doCvsDiff',
            'shortcut' => 'cd',
            'options' => array(
                'quiet' => array(
                    'shortopt' => 'q',
                    'doc' => 'Be quiet',
                    ),
                'reallyquiet' => array(
                    'shortopt' => 'Q',
                    'doc' => 'Be really quiet',
                    ),
                'date' => array(
                    'shortopt' => 'D',
                    'doc' => 'Diff against revision of DATE',
                    'arg' => 'DATE',
                    ),
                'release' => array(
                    'shortopt' => 'R',
                    'doc' => 'Diff against tag for package release REL',
                    'arg' => 'REL',
                    ),
                'revision' => array(
                    'shortopt' => 'r',
                    'doc' => 'Diff against revision REV',
                    'arg' => 'REV',
                    ),
                'context' => array(
                    'shortopt' => 'c',
                    'doc' => 'Generate context diff',
                    ),
                'unified' => array(
                    'shortopt' => 'u',
                    'doc' => 'Generate unified diff',
                    ),
                'ignore-case' => array(
                    'shortopt' => 'i',
                    'doc' => 'Ignore case, consider upper- and lower-case letters equivalent',
                    ),
                'ignore-whitespace' => array(
                    'shortopt' => 'b',
                    'doc' => 'Ignore changes in amount of white space',
                    ),
                'ignore-blank-lines' => array(
                    'shortopt' => 'B',
                    'doc' => 'Ignore changes that insert or delete blank lines',
                    ),
                'brief' => array(
                    'doc' => 'Report only whether the files differ, no details',
                    ),
                'dry-run' => array(
                    'shortopt' => 'n',
                    'doc' => 'Don\'t do anything, just pretend',
                    ),
                ),
            'doc' => '<package.xml>
Compares all the files in a package.  Without any options, this
command will compare the current code with the last checked-in code.
Using the -r or -R option you may compare the current code with that
of a specific release.
',
            ),
        'cvstag' => array(
            'summary' => 'Set CVS Release Tag',
            'function' => 'doCvsTag',
            'shortcut' => 'ct',
            'options' => array(
                'quiet' => array(
                    'shortopt' => 'q',
                    'doc' => 'Be quiet',
                    ),
                'reallyquiet' => array(
                    'shortopt' => 'Q',
                    'doc' => 'Be really quiet',
                    ),
                'slide' => array(
                    'shortopt' => 'F',
                    'doc' => 'Move (slide) tag if it exists',
                    ),
                'delete' => array(
                    'shortopt' => 'd',
                    'doc' => 'Remove tag',
                    ),
                'dry-run' => array(
                    'shortopt' => 'n',
                    'doc' => 'Don\'t do anything, just pretend',
                    ),
                ),
            'doc' => '<package.xml> [files...]
Sets a CVS tag on all files in a package.  Use this command after you have
packaged a distribution tarball with the "package" command to tag what
revisions of what files were in that release.  If need to fix something
after running cvstag once, but before the tarball is released to the public,
use the "slide" option to move the release tag.

to include files (such as a second package.xml, or tests not included in the
release), pass them as additional parameters.
',
            ),
        'package-dependencies' => array(
            'summary' => 'Show package dependencies',
            'function' => 'doPackageDependencies',
            'shortcut' => 'pd',
            'options' => array(),
            'doc' => '
List all depencies the package has.'
            ),
        'sign' => array(
            'summary' => 'Sign a package distribution file',
            'function' => 'doSign',
            'shortcut' => 'si',
            'options' => array(),
            'doc' => '<package-file>
Signs a package distribution (.tar or .tgz) file with GnuPG.',
            ),
        'makerpm' => array(
            'summary' => 'Builds an RPM spec file from a PEAR package',
            'function' => 'doMakeRPM',
            'shortcut' => 'rpm',
            'options' => array(
                'spec-template' => array(
                    'shortopt' => 't',
                    'arg' => 'FILE',
                    'doc' => 'Use FILE as RPM spec file template'
                    ),
                'rpm-pkgname' => array(
                    'shortopt' => 'p',
                    'arg' => 'FORMAT',
                    'doc' => 'Use FORMAT as format string for RPM package name, %s is replaced
by the PEAR package name, defaults to "PEAR::%s".',
                    ),
                ),
            'doc' => '<package-file>

Creates an RPM .spec file for wrapping a PEAR package inside an RPM
package.  Intended to be used from the SPECS directory, with the PEAR
package tarball in the SOURCES directory:

$ pear makerpm ../SOURCES/Net_Socket-1.0.tgz
Wrote RPM spec file PEAR::Net_Geo-1.0.spec
$ rpm -bb PEAR::Net_Socket-1.0.spec
...
Wrote: /usr/src/redhat/RPMS/i386/PEAR::Net_Socket-1.0-1.i386.rpm
',
            ),
        'convert' => array(
            'summary' => 'Convert a package.xml 1.0 to package.xml 2.0 format',
            'function' => 'doConvert',
            'shortcut' => 'c2',
            'options' => array(
                'flat' => array(
                    'shortopt' => 'f',
                    'doc' => 'do not beautify the filelist.',
                    ),
                ),
            'doc' => '[descfile] [descfile2]
Converts a package.xml in 1.0 format into a package.xml
in 2.0 format.  The new file will be named package2.xml by default,
and package.xml will be used as the old file by default.
This is not the most intelligent conversion, and should only be
used for automated conversion or learning the format.
'
            ),
        );

    var $output;

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Package constructor.
     *
     * @access public
     */
    function PEAR_Command_Package(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ _displayValidationResults()

    function _displayValidationResults($err, $warn, $strict = false)
    {
        foreach ($err as $e) {
            $this->output .= "Error: $e\n";
        }
        foreach ($warn as $w) {
            $this->output .= "Warning: $w\n";
        }
        $this->output .= sprintf('Validation: %d error(s), %d warning(s)'."\n",
                                       sizeof($err), sizeof($warn));
        if ($strict && sizeof($err) > 0) {
            $this->output .= "Fix these errors and try again.";
            return false;
        }
        return true;
    }

    // }}}
    function &getPackager()
    {
        if (!class_exists('PEAR_Packager')) {
            require_once 'PEAR/Packager.php';
        }
        $a = &new PEAR_Packager;
        return $a;
    }

    function &getPackageFile($config, $debug = false, $tmpdir = null)
    {
        if (!class_exists('PEAR_Common')) {
            require_once 'PEAR/Common.php';
        }
        if (!class_exists('PEAR/PackageFile.php')) {
            require_once 'PEAR/PackageFile.php';
        }
        $a = &new PEAR_PackageFile($config, $debug, $tmpdir);
        $common = new PEAR_Common;
        $common->ui = $this->ui;
        $a->setLogger($common);
        return $a;
    }
    // {{{ doPackage()

    function doPackage($command, $options, $params)
    {
        $this->output = '';
        $pkginfofile = isset($params[0]) ? $params[0] : 'package.xml';
        $pkg2 = isset($params[1]) ? $params[1] : null;
        if (!$pkg2 && !isset($params[0])) {
            if (file_exists('package2.xml')) {
                $pkg2 = 'package2.xml';
            }
        }
        $packager = &$this->getPackager();
        $compress = empty($options['nocompress']) ? true : false;
        $result = $packager->package($pkginfofile, $compress, $pkg2);
        if (PEAR::isError($result)) {
            return $this->raiseError($result);
        }
        // Don't want output, only the package file name just created
        if (isset($options['showname'])) {
            $this->output = $result;
        }
        if ($this->output) {
            $this->ui->outputData($this->output, $command);
        }
        return true;
    }

    // }}}
    // {{{ doPackageValidate()

    function doPackageValidate($command, $options, $params)
    {
        $this->output = '';
        if (sizeof($params) < 1) {
            $params[0] = "package.xml";
        }
        $obj = &$this->getPackageFile($this->config, $this->_debug);
        $obj->rawReturn();
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $info = $obj->fromTgzFile($params[0], PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($info)) {
            $info = $obj->fromPackageFile($params[0], PEAR_VALIDATE_NORMAL);
        } else {
            $archive = $info->getArchiveFile();
            $tar = &new Archive_Tar($archive);
            $tar->extract(dirname($info->getPackageFile()));
            $info->setPackageFile(dirname($info->getPackageFile()) . DIRECTORY_SEPARATOR .
                $info->getPackage() . '-' . $info->getVersion() . DIRECTORY_SEPARATOR .
                basename($info->getPackageFile()));
        }
        PEAR::staticPopErrorHandling();
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }
        $valid = false;
        if ($info->getPackagexmlVersion() == '2.0') {
            if ($valid = $info->validate(PEAR_VALIDATE_NORMAL)) {
                $info->flattenFileList();
                $valid = $info->validate(PEAR_VALIDATE_PACKAGING);
            }
        } else {
            $valid = $info->validate(PEAR_VALIDATE_PACKAGING);
        }
        $err = array();
        $warn = array();
        if (!$valid) {
            foreach ($info->getValidationWarnings() as $error) {
                if ($error['level'] == 'warning') {
                    $warn[] = $error['message'];
                } else {
                    $err[] = $error['message'];
                }
            }
        }
        $this->_displayValidationResults($err, $warn);
        $this->ui->outputData($this->output, $command);
        return true;
    }

    // }}}
    // {{{ doCvsTag()

    function doCvsTag($command, $options, $params)
    {
        $this->output = '';
        $_cmd = $command;
        if (sizeof($params) < 1) {
            $help = $this->getHelp($command);
            return $this->raiseError("$command: missing parameter: $help[0]");
        }
        $obj = &$this->getPackageFile($this->config, $this->_debug);
        $info = $obj->fromAnyFile($params[0], PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }
        $err = $warn = array();
        if (!$info->validate()) {
            foreach ($info->getValidationWarnings() as $error) {
                if ($error['level'] == 'warning') {
                    $warn[] = $error['message'];
                } else {
                    $err[] = $error['message'];
                }
            }
        }
        if (!$this->_displayValidationResults($err, $warn, true)) {
            $this->ui->outputData($this->output, $command);
            return $this->raiseError('CVS tag failed');
        }
        $version = $info->getVersion();
        $cvsversion = preg_replace('/[^a-z0-9]/i', '_', $version);
        $cvstag = "RELEASE_$cvsversion";
        $files = array_keys($info->getFilelist());
        $command = "cvs";
        if (isset($options['quiet'])) {
            $command .= ' -q';
        }
        if (isset($options['reallyquiet'])) {
            $command .= ' -Q';
        }
        $command .= ' tag';
        if (isset($options['slide'])) {
            $command .= ' -F';
        }
        if (isset($options['delete'])) {
            $command .= ' -d';
        }
        $command .= ' ' . $cvstag . ' ' . escapeshellarg($params[0]);
        array_shift($params);
        if (count($params)) {
            // add in additional files to be tagged
            $files = array_merge($files, $params);
        }
        foreach ($files as $file) {
            $command .= ' ' . escapeshellarg($file);
        }
        if ($this->config->get('verbose') > 1) {
            $this->output .= "+ $command\n";
        }
        $this->output .= "+ $command\n";
        if (empty($options['dry-run'])) {
            $fp = popen($command, "r");
            while ($line = fgets($fp, 1024)) {
                $this->output .= rtrim($line)."\n";
            }
            pclose($fp);
        }
        $this->ui->outputData($this->output, $_cmd);
        return true;
    }

    // }}}
    // {{{ doCvsDiff()

    function doCvsDiff($command, $options, $params)
    {
        $this->output = '';
        if (sizeof($params) < 1) {
            $help = $this->getHelp($command);
            return $this->raiseError("$command: missing parameter: $help[0]");
        }
        $obj = &$this->getPackageFile($this->config, $this->_debug);
        $info = $obj->fromAnyFile($params[0], PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }
        $err = $warn = array();
        if (!$info->validate()) {
            foreach ($info->getValidationWarnings() as $error) {
                if ($error['level'] == 'warning') {
                    $warn[] = $error['message'];
                } else {
                    $err[] = $error['message'];
                }
            }
        }
        if (!$this->_displayValidationResults($err, $warn, true)) {
            $this->ui->outputData($this->output, $command);
            return $this->raiseError('CVS diff failed');
        }
        $info1 = $info->getFilelist();
        $files = $info1;
        $cmd = "cvs";
        if (isset($options['quiet'])) {
            $cmd .= ' -q';
            unset($options['quiet']);
        }
        if (isset($options['reallyquiet'])) {
            $cmd .= ' -Q';
            unset($options['reallyquiet']);
        }
        if (isset($options['release'])) {
            $cvsversion = preg_replace('/[^a-z0-9]/i', '_', $options['release']);
            $cvstag = "RELEASE_$cvsversion";
            $options['revision'] = $cvstag;
            unset($options['release']);
        }
        $execute = true;
        if (isset($options['dry-run'])) {
            $execute = false;
            unset($options['dry-run']);
        }
        $cmd .= ' diff';
        // the rest of the options are passed right on to "cvs diff"
        foreach ($options as $option => $optarg) {
            $arg = @$this->commands[$command]['options'][$option]['arg'];
            $short = @$this->commands[$command]['options'][$option]['shortopt'];
            $cmd .= $short ? " -$short" : " --$option";
            if ($arg && $optarg) {
                $cmd .= ($short ? '' : '=') . escapeshellarg($optarg);
            }
        }
        foreach ($files as $file) {
            $cmd .= ' ' . escapeshellarg($file['name']);
        }
        if ($this->config->get('verbose') > 1) {
            $this->output .= "+ $cmd\n";
        }
        if ($execute) {
            $fp = popen($cmd, "r");
            while ($line = fgets($fp, 1024)) {
                $this->output .= rtrim($line)."\n";
            }
            pclose($fp);
        }
        $this->ui->outputData($this->output, $command);
        return true;
    }

    // }}}
    // {{{ doPackageDependencies()

    function doPackageDependencies($command, $options, $params)
    {
        // $params[0] -> the PEAR package to list its information
        if (sizeof($params) != 1) {
            return $this->raiseError("bad parameter(s), try \"help $command\"");
        }
        $obj = &$this->getPackageFile($this->config, $this->_debug);
        $info = $obj->fromAnyFile($params[0], PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }
        $deps = $info->getDeps();
        if (is_array($deps)) {
            if ($info->getPackagexmlVersion() == '1.0') {
                $data = array(
                    'caption' => 'Dependencies for pear/' . $info->getPackage(),
                    'border' => true,
                    'headline' => array("Required?", "Type", "Name", "Relation", "Version"),
                    );

                foreach ($deps as $d) {
                    if (isset($d['optional'])) {
                        if ($d['optional'] == 'yes') {
                            $req = 'No';
                        } else {
                            $req = 'Yes';
                        }
                    } else {
                        $req = 'Yes';
                    }
                    if (isset($this->_deps_rel_trans[$d['rel']])) {
                        $rel = $this->_deps_rel_trans[$d['rel']];
                    } else {
                        $rel = $d['rel'];
                    }

                    if (isset($this->_deps_type_trans[$d['type']])) {
                        $type = ucfirst($this->_deps_type_trans[$d['type']]);
                    } else {
                        $type = $d['type'];
                    }

                    if (isset($d['name'])) {
                        $name = $d['name'];
                    } else {
                        $name = '';
                    }

                    if (isset($d['version'])) {
                        $version = $d['version'];
                    } else {
                        $version = '';
                    }

                    $data['data'][] = array($req, $type, $name, $rel, $version);
                }
            } else { // package.xml 2.0 dependencies display
                require_once 'PEAR/Dependency2.php';
                $deps = $info->getDependencies();
                $reg = &$this->config->getRegistry();
                if (is_array($deps)) {
                    $d = new PEAR_Dependency2($this->config, array(), '');
                    $data = array(
                        'caption' => 'Dependencies for ' . $info->getPackage(),
                        'border' => true,
                        'headline' => array("Required?", "Type", "Name", 'Versioning', 'Group'),
                        );
                    foreach ($deps as $type => $subd) {
                        $req = ($type == 'required') ? 'Yes' : 'No';
                        if ($type == 'group') {
                            $group = $subd['attribs']['name'];
                        } else {
                            $group = '';
                        }
                        if (!isset($subd[0])) {
                            $subd = array($subd);
                        }
                        foreach ($subd as $groupa) {
                            foreach ($groupa as $deptype => $depinfo) {
                                if ($deptype == 'attribs') {
                                    continue;
                                }
                                if ($deptype == 'pearinstaller') {
                                    $deptype = 'pear Installer';
                                }
                                if (!isset($depinfo[0])) {
                                    $depinfo = array($depinfo);
                                }
                                foreach ($depinfo as $inf) {
                                    $name = '';
                                    if (isset($inf['channel'])) {
                                        $alias = $reg->channelAlias($inf['channel']);
                                        if (!$alias) {
                                            $alias = '(channel?) ' .$inf['channel'];
                                        }
                                        $name = $alias . '/';
                                    }
                                    if (isset($inf['name'])) {
                                        $name .= $inf['name'];
                                    } elseif (isset($inf['pattern'])) {
                                        $name .= $inf['pattern'];
                                    } else {
                                        $name .= '';
                                    }
                                    if (isset($inf['uri'])) {
                                        $name .= ' [' . $inf['uri'] .  ']';
                                    }
                                    if (isset($inf['conflicts'])) {
                                        $ver = 'conflicts';
                                    } else {
                                        $ver = $d->_getExtraString($inf);
                                    }
                                    $data['data'][] = array($req, ucfirst($deptype), $name,
                                        $ver, $group);
                                }
                            }
                        }
                    }
                }
            }

            $this->ui->outputData($data, $command);
            return true;
        }

        // Fallback
        $this->ui->outputData("This package does not have any dependencies.", $command);
    }

    // }}}
    // {{{ doSign()

    function doSign($command, $options, $params)
    {
        require_once 'System.php';
        require_once 'Archive/Tar.php';
        // should move most of this code into PEAR_Packager
        // so it'll be easy to implement "pear package --sign"
        if (sizeof($params) != 1) {
            return $this->raiseError("bad parameter(s), try \"help $command\"");
        }
        if (!file_exists($params[0])) {
            return $this->raiseError("file does not exist: $params[0]");
        }
        $obj = $this->getPackageFile($this->config, $this->_debug);
        $info = $obj->fromTgzFile($params[0], PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }
        $tar = new Archive_Tar($params[0]);
        $tmpdir = System::mktemp('-d pearsign');
        if (!$tar->extractList('package2.xml package.sig', $tmpdir)) {
            if (!$tar->extractList('package.xml package.sig', $tmpdir)) {
                return $this->raiseError("failed to extract tar file");
            }
        }
        if (file_exists("$tmpdir/package.sig")) {
            return $this->raiseError("package already signed");
        }
        $packagexml = 'package.xml';
        if (file_exists("$tmpdir/package2.xml")) {
            $packagexml = 'package2.xml';
        }
        @unlink("$tmpdir/package.sig");
        $input = $this->ui->userDialog($command,
                                       array('GnuPG Passphrase'),
                                       array('password'));
        $gpg = popen("gpg --batch --passphrase-fd 0 --armor --detach-sign --output $tmpdir/package.sig $tmpdir/$packagexml 2>/dev/null", "w");
        if (!$gpg) {
            return $this->raiseError("gpg command failed");
        }
        fwrite($gpg, "$input[0]\n");
        if (pclose($gpg) || !file_exists("$tmpdir/package.sig")) {
            return $this->raiseError("gpg sign failed");
        }
        $tar->addModify("$tmpdir/package.sig", '', $tmpdir);
        return true;
    }

    // }}}

    /**
     * For unit testing purposes
     */
    function &getInstaller(&$ui)
    {
        if (!class_exists('PEAR_Installer')) {
            require_once 'PEAR/Installer.php';
        }
        $a = &new PEAR_Installer($ui);
        return $a;
    }

    // {{{ doMakeRPM()

    /*

    (cox)

    TODO:
        - Fill the rpm dependencies in the template file.

    IDEAS:
        - Instead of mapping the role to rpm vars, perhaps it's better
          to use directly the pear cmd to install the files by itself
          in %postrun so:
          pear -d php_dir=%{_libdir}/php/pear -d test_dir=.. <package>
    */

    function doMakeRPM($command, $options, $params)
    {
        require_once 'System.php';
        require_once 'Archive/Tar.php';
        if (sizeof($params) != 1) {
            return $this->raiseError("bad parameter(s), try \"help $command\"");
        }
        if (!file_exists($params[0])) {
            return $this->raiseError("file does not exist: $params[0]");
        }
        $reg = &$this->config->getRegistry();
        $pkg = &$this->getPackageFile($this->config, $this->_debug);
        $pf = &$pkg->fromAnyFile($params[0], PEAR_VALIDATE_NORMAL);
        if (PEAR::isError($pf)) {
            $u = $pf->getUserinfo();
            if (is_array($u)) {
                foreach ($u as $err) {
                    if (is_array($err)) {
                        $err = $err['message'];
                    }
                    $this->ui->outputData($err);
                }
            }
            return $this->raiseError("$params[0] is not a valid package");
        }
        $tmpdir = System::mktemp(array('-d', 'pear2rpm'));
        $instroot = System::mktemp(array('-d', 'pear2rpm'));
        $tmp = $this->config->get('verbose');
        $this->config->set('verbose', 0);
        $installer = $this->getInstaller($this->ui);
        require_once 'PEAR/Downloader/Package.php';
        $pack = new PEAR_Downloader_Package($installer);
        $pack->setPackageFile($pf);
        $params[0] = &$pack;
        $installer->setOptions(array('installroot' => $instroot,
                                          'nodeps' => true, 'soft' => true));
        $installer->setDownloadedPackages($params);
        $info = $installer->install($params[0],
                                    array('installroot' => $instroot,
                                          'nodeps' => true, 'soft' => true));
        $pkgdir = $pf->getPackage() . '-' . $pf->getVersion();
        $info['rpm_xml_dir'] = '/var/lib/pear';
        $this->config->set('verbose', $tmp);
        if (isset($options['spec-template'])) {
            $spec_template = $options['spec-template'];
        } else {
            $spec_template = '@DATA-DIR@/PEAR/template.spec';
        }
        $info['possible_channel'] = '';
        $info['extra_config'] = '';
        if (isset($options['rpm-pkgname'])) {
            $rpm_pkgname_format = $options['rpm-pkgname'];
        } else {
            if ($pf->getChannel() == 'pear.php.net' || $pf->getChannel() == 'pecl.php.net') {
                $alias = 'PEAR';
            } else {
                $chan = &$reg->getChannel($pf->getChannel());
                $alias = $chan->getAlias();
                $alias = strtoupper($alias);
                $info['possible_channel'] = $pf->getChannel() . '/';
            }
            $rpm_pkgname_format = $alias . '::%s';
        }

        $info['extra_headers'] = '';
        $info['doc_files'] = '';
        $info['files'] = '';
        $info['package2xml'] = '';
        $info['rpm_package'] = sprintf($rpm_pkgname_format, $pf->getPackage());
        $srcfiles = 0;
        foreach ($info['filelist'] as $name => $attr) {
            if (!isset($attr['role'])) {
                continue;
            }
            $name = preg_replace('![/:\\\\]!', '/', $name);
            if ($attr['role'] == 'doc') {
                $info['doc_files'] .= " $name";
            // Map role to the rpm vars
            } else {
                $c_prefix = '%{_libdir}/php/pear';
                switch ($attr['role']) {
                    case 'php':
                        $prefix = $c_prefix;
                    break;
                    case 'ext':
                        $prefix = '%{_libdir}/php';
                    break; // XXX good place?
                    case 'src':
                        $srcfiles++;
                        $prefix = '%{_includedir}/php';
                    break; // XXX good place?
                    case 'test':
                        $prefix = "$c_prefix/tests/" . $pf->getPackage();
                    break;
                    case 'data':
                        $prefix = "$c_prefix/data/" . $pf->getPackage();
                    break;
                    case 'script':
                        $prefix = '%{_bindir}';
                    break;
                    default: // non-standard roles
                        $prefix = "$c_prefix/$attr[role]/" . $pf->getPackage();
                        $info['extra_config'] .=
                        "\n        -d {$attr[role]}_dir=$c_prefix/{$attr[role]} \\";
                        $this->ui->outputData('WARNING: role "' . $attr['role'] . '" used, ' .
                            'and will be installed in "' . $c_prefix . '/' . $attr['role'] .
                            '/' . $pf->getPackage() .
                            ' - hand-edit the final .spec if this is wrong', $command);
                    break;
                }
                $name = str_replace('\\', '/', $name);
                $info['files'] .= "$prefix/$name\n";
            }
        }
        if ($srcfiles > 0) {
            require_once 'OS/Guess.php';
            $os = new OS_Guess;
            $arch = $os->getCpu();
        } else {
            $arch = 'noarch';
        }
        $cfg = array('master_server', 'php_dir', 'ext_dir', 'doc_dir',
                     'bin_dir', 'data_dir', 'test_dir');
        foreach ($cfg as $k) {
            if ($k == 'master_server') {
                $chan = $reg->getChannel($pf->getChannel());
                $info[$k] = $chan->getServer();
                continue;
            }
            $info[$k] = $this->config->get($k);
        }
        $info['arch'] = $arch;
        $fp = @fopen($spec_template, "r");
        if (!$fp) {
            return $this->raiseError("could not open RPM spec file template $spec_template: $php_errormsg");
        }
        $info['package'] = $pf->getPackage();
        $info['version'] = $pf->getVersion();
        $info['release_license'] = $pf->getLicense();
        if ($pf->getDeps()) {
            if ($pf->getPackagexmlVersion() == '1.0') {
                $requires = $conflicts = array();
                foreach ($pf->getDeps() as $dep) {
                    if (isset($dep['optional']) && $dep['optional'] == 'yes') {
                        continue;
                    }
                    if ($dep['type'] != 'pkg') {
                        continue;
                    }
                    if (isset($dep['channel']) && $dep['channel'] != 'pear.php.net' &&
                          $dep['channel'] != 'pecl.php.net') {
                        $chan = &$reg->getChannel($dep['channel']);
                        $package = strtoupper($chan->getAlias()) . '::' . $dep['name'];
                    } else {
                        $package = 'PEAR::' . $dep['name'];
                    }
                    $trans = array(
                        '>' => '>',
                        '<' => '<',
                        '>=' => '>=',
                        '<=' => '<=',
                        '=' => '=',
                        'gt' => '>',
                        'lt' => '<',
                        'ge' => '>=',
                        'le' => '<=',
                        'eq' => '=',
                    );
                    if ($dep['rel'] == 'has') {
                        $requires[] = $package;
                    } elseif ($dep['rel'] == 'not') {
                        $conflicts[] = $package;
                    } elseif ($dep['rel'] == 'ne') {
                        $conflicts[] = $package . ' = ' . $dep['version'];
                    } elseif (isset($trans[$dep['rel']])) {
                        $requires[] = $package . ' ' . $trans[$dep['rel']] . ' ' . $dep['version'];
                    }
                }
                if (count($requires)) {
                    $info['extra_headers'] .= 'Requires: ' . implode(', ', $requires) . "\n";
                }
                if (count($conflicts)) {
                    $info['extra_headers'] .= 'Conflicts: ' . implode(', ', $conflicts) . "\n";
                }
            } else {
                $info['package2xml'] = '2'; // tell the spec to use package2.xml
                $requires = $conflicts = array();
                $deps = $pf->getDeps(true);
                if (isset($deps['required']['package'])) {
                    if (!isset($deps['required']['package'][0])) {
                        $deps['required']['package'] = array($deps['required']['package']);
                    }
                    foreach ($deps['required']['package'] as $dep) {
                        if ($dep['channel'] != 'pear.php.net' &&  $dep['channel'] != 'pecl.php.net') {
                            $chan = &$reg->getChannel($dep['channel']);
                            $package = strtoupper($chan->getAlias()) . '::' . $dep['name'];
                        } else {
                            $package = 'PEAR::' . $dep['name'];
                        }
                        if (isset($dep['conflicts']) && (isset($dep['min']) ||
                              isset($dep['max']))) {
                            $deprange = array();
                            if (isset($dep['min'])) {
                                $deprange[] = array($dep['min'],'>=');
                            }
                            if (isset($dep['max'])) {
                                $deprange[] = array($dep['max'], '<=');
                            }
                            if (isset($dep['exclude'])) {
                                if (!is_array($dep['exclude']) ||
                                      !isset($dep['exclude'][0])) {
                                    $dep['exclude'] = array($dep['exclude']);
                                }
                                if (count($deprange)) {
                                    $excl = $dep['exclude'];
                                    // change >= to > if excluding the min version
                                    // change <= to < if excluding the max version
                                    for($i = 0; $i < count($excl); $i++) {
                                        if (isset($deprange[0]) &&
                                              $excl[$i] == $deprange[0][0]) {
                                            $deprange[0][1] = '<';
                                            unset($dep['exclude'][$i]);
                                        }
                                        if (isset($deprange[1]) &&
                                              $excl[$i] == $deprange[1][0]) {
                                            $deprange[1][1] = '>';
                                            unset($dep['exclude'][$i]);
                                        }
                                    }
                                }
                                if (count($dep['exclude'])) {
                                    $dep['exclude'] = array_values($dep['exclude']);
                                    $newdeprange = array();
                                    // remove excludes that are outside the existing range
                                    for ($i = 0; $i < count($dep['exclude']); $i++) {
                                        if ($dep['exclude'][$i] < $dep['min'] ||
                                              $dep['exclude'][$i] > $dep['max']) {
                                            unset($dep['exclude'][$i]);
                                        }
                                    }
                                    $dep['exclude'] = array_values($dep['exclude']);
                                    usort($dep['exclude'], 'version_compare');
                                    // take the remaining excludes and
                                    // split the dependency into sub-ranges
                                    $lastmin = $deprange[0];
                                    for ($i = 0; $i < count($dep['exclude']) - 1; $i++) {
                                        $newdeprange[] = '(' .
                                            $package . " {$lastmin[1]} {$lastmin[0]} and " .
                                            $package . ' < ' . $dep['exclude'][$i] . ')';
                                        $lastmin = array($dep['exclude'][$i], '>');
                                    }
                                    if (isset($dep['max'])) {
                                        $newdeprange[] = '(' . $package .
                                            " {$lastmin[1]} {$lastmin[0]} and " .
                                            $package . ' < ' . $dep['max'] . ')';
                                    }
                                    $conflicts[] = implode(' or ', $deprange);
                                } else {
                                    $conflicts[] = $package .
                                        " {$deprange[0][1]} {$deprange[0][0]}" .
                                        (isset($deprange[1]) ? 
                                        " and $package {$deprange[1][1]} {$deprange[1][0]}"
                                        : '');
                                }
                            }
                            continue;
                        }
                        if (!isset($dep['min']) && !isset($dep['max']) &&
                              !isset($dep['exclude'])) {
                            if (isset($dep['conflicts'])) {
                                $conflicts[] = $package;
                            } else {
                                $requires[] = $package;
                            }
                        } else {
                            if (isset($dep['min'])) {
                                $requires[] = $package . ' >= ' . $dep['min'];
                            }
                            if (isset($dep['max'])) {
                                $requires[] = $package . ' <= ' . $dep['max'];
                            }
                            if (isset($dep['exclude'])) {
                                $ex = $dep['exclude'];
                                if (!is_array($ex)) {
                                    $ex = array($ex);
                                }
                                foreach ($ex as $ver) {
                                    $conflicts[] = $package . ' = ' . $ver;
                                }
                            }
                        }
                    }
                    require_once 'Archive/Tar.php';
                    $tar = new Archive_Tar($pf->getArchiveFile());
                    $tar->pushErrorHandling(PEAR_ERROR_RETURN);
                    $a = $tar->extractInString('package2.xml');
                    $tar->popErrorHandling();
                    if ($a === null || PEAR::isError($a)) {
                        $info['package2xml'] = '';
                        // this doesn't have a package.xml version 1.0
                        $requires[] = 'PEAR::PEAR >= ' .
                            $deps['required']['pearinstaller']['min'];
                    }
                    if (count($requires)) {
                        $info['extra_headers'] .= 'Requires: ' . implode(', ', $requires) . "\n";
                    }
                    if (count($conflicts)) {
                        $info['extra_headers'] .= 'Conflicts: ' . implode(', ', $conflicts) . "\n";
                    }
                }
            }
        }

        // remove the trailing newline
        $info['extra_headers'] = trim($info['extra_headers']);
        if (function_exists('file_get_contents')) {
            fclose($fp);
            $spec_contents = preg_replace('/@([a-z0-9_-]+)@/e', '$info["\1"]',
                file_get_contents($spec_template));
        } else {
            $spec_contents = preg_replace('/@([a-z0-9_-]+)@/e', '$info["\1"]',
                fread($fp, filesize($spec_template)));
            fclose($fp);
        }
        $spec_file = "$info[rpm_package]-$info[version].spec";
        $wp = fopen($spec_file, "wb");
        if (!$wp) {
            return $this->raiseError("could not write RPM spec file $spec_file: $php_errormsg");
        }
        fwrite($wp, $spec_contents);
        fclose($wp);
        $this->ui->outputData("Wrote RPM spec file $spec_file", $command);

        return true;
    }

    function doConvert($command, $options, $params)
    {
        $packagexml = isset($params[0]) ? $params[0] : 'package.xml';
        $newpackagexml = isset($params[1]) ? $params[1] : dirname($packagexml) .
            DIRECTORY_SEPARATOR . 'package2.xml';
        $pkg = &$this->getPackageFile($this->config, $this->_debug);
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $pf = $pkg->fromPackageFile($packagexml, PEAR_VALIDATE_NORMAL);
        PEAR::staticPopErrorHandling();
        if (!PEAR::isError($pf)) {
            if (is_a($pf, 'PEAR_PackageFile_v2')) {
                $this->ui->outputData($packagexml . ' is already a package.xml version 2.0');
                return true;
            }
            $gen = &$pf->getDefaultGenerator();
            $newpf = &$gen->toV2();
            $newpf->setPackagefile($newpackagexml);
            $gen = &$newpf->getDefaultGenerator();
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $state = (isset($options['flat']) ? PEAR_VALIDATE_PACKAGING : PEAR_VALIDATE_NORMAL);
            $saved = $gen->toPackageFile(dirname($newpackagexml), $state,
                basename($newpackagexml));
            PEAR::staticPopErrorHandling();
            if (PEAR::isError($saved)) {
                if (is_array($saved->getUserInfo())) {
                    foreach ($saved->getUserInfo() as $warning) {
                        $this->ui->outputData($warning['message']);
                    }
                }
                $this->ui->outputData($saved->getMessage());
                return true;
            }
            $this->ui->outputData('Wrote new version 2.0 package.xml to "' . $saved . '"');
            return true;
        } else {
            if (is_array($pf->getUserInfo())) {
                foreach ($pf->getUserInfo() as $warning) {
                    $this->ui->outputData($warning['message']);
                }
            }
            return $this->raiseError($pf);
        }
    }

    // }}}
}

?>
