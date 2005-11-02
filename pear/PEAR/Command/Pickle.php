<?php
/**
 * PEAR_Command_Pickle (pickle command)
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
 * @copyright  2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.1
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
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.1
 */

class PEAR_Command_Pickle extends PEAR_Command_Common
{
    var $commands = array(
        'pickle' => array(
            'summary' => 'Build PECL Package',
            'function' => 'doPackage',
            'shortcut' => 'pi',
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
            'doc' => '[descfile]
Creates a PECL package from its package2.xml file.

An automatic conversion will be made to a package.xml 1.0 and written out to
disk in the current directory as "package.xml".  Note that
only simple package.xml 2.0 will be converted.  package.xml 2.0 with:

 - dependency types other than required/optional PECL package/ext/php/pearinstaller
 - more than one extsrcrelease
 - extbinrelease, phprelease, or bundle release type
 - dependency groups
 - ignore tags in release filelist
 - tasks other than replace
 - custom roles

will cause pickle to fail, and output an error message.  If your package2.xml
uses any of these features, you are best off using PEAR_PackageFileManager to
generate both package.xml.
'
            ),
        );

    /**
     * PEAR_Command_Package constructor.
     *
     * @access public
     */
    function PEAR_Command_Pickle(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }


    /**
     * For unit-testing ease
     *
     * @return PEAR_Packager
     */
    function &getPackager()
    {
        if (!class_exists('PEAR_Packager')) {
            require_once 'PEAR/Packager.php';
        }
        $a = &new PEAR_Packager;
        return $a;
    }

    /**
     * For unit-testing ease
     *
     * @param PEAR_Config $config
     * @param bool $debug
     * @param string|null $tmpdir
     * @return PEAR_PackageFile
     */
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

    function doPackage($command, $options, $params)
    {
        $this->output = '';
        $pkginfofile = isset($params[0]) ? $params[0] : 'package2.xml';
        $packager = &$this->getPackager();
        if (PEAR::isError($err = $this->_convertPackage($pkginfofile))) {
            return $err;
        }
        $compress = empty($options['nocompress']) ? true : false;
        $result = $packager->package($pkginfofile, $compress, 'package.xml');
        if (PEAR::isError($result)) {
            return $this->raiseError($result);
        }
        // Don't want output, only the package file name just created
        if (isset($options['showname'])) {
            $this->ui->outputData($result, $command);
        }
        return true;
    }

    function _convertPackage($packagexml)
    {
        $pkg = &$this->getPackageFile($this->config);
        $pf2 = &$pkg->fromPackageFile($packagexml, PEAR_VALIDATE_NORMAL);
        if (!is_a($pf2, 'PEAR_PackageFile_v2')) {
            return $this->raiseError('Cannot process "' .
                $packagexml . '", is not a package.xml 2.0');
        }
        require_once 'PEAR/PackageFile/v1.php';
        $pf = new PEAR_PackageFile_v1;
        $pf->setConfig($this->config);
        if (is_array($pf2->getPackageType() != 'extsrc')) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", is not an extension source package.  Using a PEAR_PackageFileManager-based ' .
            'script is an option');
        }
        if (is_array($pf2->getUsesRole())) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", contains custom roles.  Using a PEAR_PackageFileManager-based script or ' .
            'the convert command is an option');
        }
        if (is_array($pf2->getUsesTask())) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", contains custom tasks.  Using a PEAR_PackageFileManager-based script or ' .
            'the convert command is an option');
        }
        $deps = $pf2->getDependencies();
        if (isset($deps['group'])) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", contains dependency groups.  Using a PEAR_PackageFileManager-based script ' .
            'or the convert command is an option');
        }
        if (isset($deps['required']['subpackage']) ||
              isset($deps['optional']['subpackage'])) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", contains subpackage dependencies.  Using a PEAR_PackageFileManager-based  '.
            'script is an option');
        }
        if (isset($deps['required']['os'])) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", contains os dependencies.  Using a PEAR_PackageFileManager-based  '.
            'script is an option');
        }
        if (isset($deps['required']['arch'])) {
            return $this->raiseError('Cannot safely convert "' . $packagexml .
            '", contains arch dependencies.  Using a PEAR_PackageFileManager-based  '.
            'script is an option');
        }
        $pf->setPackage($pf2->getPackage());
        $pf->setSummary($pf2->getSummary());
        $pf->setDescription($pf2->getDescription());
        foreach ($pf2->getMaintainers() as $maintainer) {
            $pf->addMaintainer($maintainer['role'], $maintainer['handle'],
                $maintainer['name'], $maintainer['email']);
        }
        $pf->setVersion($pf2->getVersion());
        $pf->setDate($pf2->getDate());
        $pf->setLicense($pf2->getLicense());
        $pf->setState($pf2->getState());
        $pf->setNotes($pf2->getNotes());
        $pf->addPhpDep($deps['required']['php']['min'], 'ge');
        if (isset($deps['required']['php']['max'])) {
            $pf->addPhpDep($deps['required']['php']['max'], 'le');
        }
        if (isset($deps['required']['package'])) {
            if (!isset($deps['required']['package'][0])) {
                $deps['required']['package'] = array($deps['required']['package']);
            }
            foreach ($deps['required']['package'] as $dep) {
                if (!isset($dep['channel'])) {
                    return $this->raiseError('Cannot safely convert "' . $packagexml . '"' .
                    ' contains uri-based dependency on a package.  Using a ' .
                    'PEAR_PackageFileManager-based script is an option');
                }
                if ($dep['channel'] != 'pear.php.net' && $dep['channel'] != 'pecl.php.net') {
                    return $this->raiseError('Cannot safely convert "' . $packagexml . '"' .
                    ' contains dependency on a non-standard channel package.  Using a ' .
                    'PEAR_PackageFileManager-based script is an option');
                }
                if (isset($dep['conflicts'])) {
                    return $this->raiseError('Cannot safely convert "' . $packagexml . '"' .
                    ' contains conflicts dependency.  Using a ' .
                    'PEAR_PackageFileManager-based script is an option');
                }
                if (isset($dep['exclude'])) {
                    $this->ui->outputData('WARNING: exclude tags are ignored in conversion');
                }
                if (isset($dep['min'])) {
                    $pf->addPackageDep($dep['name'], $dep['min'], 'ge');
                }
                if (isset($dep['max'])) {
                    $pf->addPackageDep($dep['name'], $dep['max'], 'le');
                }
            }
        }
        if (isset($deps['required']['extension'])) {
            if (!isset($deps['required']['extension'][0])) {
                $deps['required']['extension'] = array($deps['required']['extension']);
            }
            foreach ($deps['required']['extension'] as $dep) {
                if (isset($dep['conflicts'])) {
                    return $this->raiseError('Cannot safely convert "' . $packagexml . '"' .
                    ' contains conflicts dependency.  Using a ' .
                    'PEAR_PackageFileManager-based script is an option');
                }
                if (isset($dep['exclude'])) {
                    $this->ui->outputData('WARNING: exclude tags are ignored in conversion');
                }
                if (isset($dep['min'])) {
                    $pf->addExtensionDep($dep['name'], $dep['min'], 'ge');
                }
                if (isset($dep['max'])) {
                    $pf->addExtensionDep($dep['name'], $dep['max'], 'le');
                }
            }
        }
        if (isset($deps['optional']['package'])) {
            if (!isset($deps['optional']['package'][0])) {
                $deps['optional']['package'] = array($deps['optional']['package']);
            }
            foreach ($deps['optional']['package'] as $dep) {
                if (!isset($dep['channel'])) {
                    return $this->raiseError('Cannot safely convert "' . $packagexml . '"' .
                    ' contains uri-based dependency on a package.  Using a ' .
                    'PEAR_PackageFileManager-based script is an option');
                }
                if ($dep['channel'] != 'pear.php.net' && $dep['channel'] != 'pecl.php.net') {
                    return $this->raiseError('Cannot safely convert "' . $packagexml . '"' .
                    ' contains dependency on a non-standard channel package.  Using a ' .
                    'PEAR_PackageFileManager-based script is an option');
                }
                if (isset($dep['exclude'])) {
                    $this->ui->outputData('WARNING: exclude tags are ignored in conversion');
                }
                if (isset($dep['min'])) {
                    $pf->addPackageDep($dep['name'], $dep['min'], 'ge', 'yes');
                }
                if (isset($dep['max'])) {
                    $pf->addPackageDep($dep['name'], $dep['max'], 'le', 'yes');
                }
            }
        }
        if (isset($deps['optional']['extension'])) {
            if (!isset($deps['optional']['extension'][0])) {
                $deps['optional']['extension'] = array($deps['optional']['extension']);
            }
            foreach ($deps['optional']['extension'] as $dep) {
                if (isset($dep['exclude'])) {
                    $this->ui->outputData('WARNING: exclude tags are ignored in conversion');
                }
                if (isset($dep['min'])) {
                    $pf->addExtensionDep($dep['name'], $dep['min'], 'ge', 'yes');
                }
                if (isset($dep['max'])) {
                    $pf->addExtensionDep($dep['name'], $dep['max'], 'le', 'yes');
                }
            }
        }
        $contents = $pf2->getContents();
        $release = $pf2->getReleases();
        if (isset($releases[0])) {
            return $this->raiseError('Cannot safely process "' . $packagexml . '" contains ' 
            . 'multiple extsrcrelease tags.  Using a PEAR_PackageFileManager-based script ' .
            'or the convert command is an option');
        }
        if ($configoptions = $pf2->getConfigureOptions()) {
            foreach ($configoptions as $option) {
                $pf->addConfigureOption($option['name'], $option['prompt'],
                    isset($option['default']) ? $option['default'] : false);
            }
        }
        if (isset($release['filelist']['ignore'])) {
            return $this->raiseError('Cannot safely process "' . $packagexml . '" contains ' 
            . 'ignore tags.  Using a PEAR_PackageFileManager-based script or the convert' .
            ' command is an option');
        }
        if (isset($release['filelist']['install']) &&
              !isset($release['filelist']['install'][0])) {
            $release['filelist']['install'] = array($release['filelist']['install']);
        }
        if (isset($contents['dir']['attribs']['baseinstalldir'])) {
            $baseinstalldir = $contents['dir']['attribs']['baseinstalldir'];
        } else {
            $baseinstalldir = false;
        }
        if (!isset($contents['dir']['file'][0])) {
            $contents['dir']['file'] = array($contents['dir']['file']);
        }
        foreach ($contents['dir']['file'] as $file) {
            if ($baseinstalldir && !isset($file['attribs']['baseinstalldir'])) {
                $file['attribs']['baseinstalldir'] = $baseinstalldir;
            }
            $processFile = $file;
            unset($processFile['attribs']);
            if (count($processFile)) {
                foreach ($processFile as $name => $task) {
                    if ($name != $pf2->getTasksNs() . ':replace') {
                        return $this->raiseError('Cannot safely process "' . $packagexml .
                        '" contains tasks other than replace.  Using a ' .
                        'PEAR_PackageFileManager-based script is an option.');
                    }
                    $file['attribs']['replace'][] = $task;
                }
            }
            if (!in_array($file['attribs']['role'], PEAR_Common::getFileRoles())) {
                return $this->raiseError('Cannot safely convert "' . $packagexml .
                '", contains custom roles.  Using a PEAR_PackageFileManager-based script ' .
                'or the convert command is an option');
            }
            if (isset($release['filelist']['install'])) {
                foreach ($release['filelist']['install'] as $installas) {
                    if ($installas['attribs']['name'] == $file['attribs']['name']) {
                        $file['attribs']['install-as'] = $installas['attribs']['as'];
                    }
                }
            }
            $pf->addFile('/', $file['attribs']['name'], $file['attribs']);
        }
        if ($pf2->getChangeLog()) {
            $this->ui->outputData('WARNING: changelog is not translated to package.xml ' .
                '1.0, use PEAR_PackageFileManager-based script if you need changelog-' .
                'translation for package.xml 1.0');
        }
        $gen = &$pf->getDefaultGenerator();
        $gen->toPackageFile('.');
    }
}

?>
