<?php

$notes = '
Major feature functionality release
 * add support for tar-based and zip-based phar archives [Greg]
 * add Phar::isTar(), Phar::isZip(), and Phar::isPhar() [Greg]
 * add Phar::convertToTar(), Phar::convertToZip(), and Phar::convertToPhar() [Greg]
 * add Phar::webPhar() for running a web-based application unmodified
   directly from a phar archive [Greg]
 * file functions (fopen-based and stat-based) can be instructed to only look for
   relative paths within a phar via Phar::interceptFileFuncs()
 * include works unmodified within a phar [Greg]
 * paths with . and .. work (phar://blah.phar/a/../b.php => phar://blah.phar/b.php) [Greg]
 * add support for mkdir()/rmdir() and support for empty directories to phar file format [Greg]
 * add option to compress the entire phar file for phar/tar file format [Greg]
 * implement Phar::isCompressed() returning 0, Phar::GZ or Phar::BZ2 [Greg]
 * implement Phar::copy(string $from, string $to) [Greg]
 * implement Phar::buildFromIterator(Iterator $it[, string $base_directory]) [Greg]
 * add mapping of include/require from within a phar to location within phar [Greg]
   solves the include_path issue without code munging
 * add Phar::delete() [Greg]
';

if (!class_exists("Phar") && !extension_loaded("Phar")) {
	die("Extension phar not present");
}

require_once 'PEAR/PackageFileManager2.php';

PEAR::setErrorHandling(PEAR_ERROR_DIE);

$options = array(
    'filelistgenerator' => 'CVS',
    'changelogoldtonew' => false,
    'simpleoutput'      => true,
    'baseinstalldir'    => '/',
    'packagedirectory'  => dirname(__FILE__),
    'packagefile'       => 'package.xml',
    'clearcontents'     => true,
    'ignore'            => array('package*.php', 'package*.xml'),
    'dir_roles'         => array(
         'docs'         => 'doc',
         'examples'     => 'doc',
         'tests'        => 'test',
         'phar'         => 'src',
    ),
    'exceptions'        => array(
         'CREDITS'            => 'doc',
         'EXPERIMENTAL'       => 'doc',
         'LICENSE'            => 'doc',
         'Makefile.frag'      => 'src',
         'phar_path_check.re' => 'src',
         'TODO'               => 'doc',
         'phar.phar'          => 'script',
    ),
);

$package = PEAR_PackageFileManager2::importOptions(dirname(__FILE__) . '/package.xml', $options);

$package->clearDeps();
$package->setPhpDep('5.2.0');
$package->setPearInstallerDep('1.4.3');
$package->addPackageDepWithChannel('optional', 'bz2', 'pecl.php.net', false, false, false, false, 'bz2');
// all this false business sets the <providesextension> tag that allows us to have hash built
// in statically
$package->addPackageDepWithChannel('optional', 'hash', 'pecl.php.net', false, false, false, false, 'hash');
$package->addExtensionDep('optional', 'spl');
$package->addExtensionDep('optional', 'zlib');
$package->setPackageType('extsrc');
$package->addRelease();
$package->setReleaseVersion(phpversion('phar'));
$package->setAPIVersion(Phar::apiVersion());
$package->setReleaseStability('stable');
$package->setAPIStability('stable');
$package->setNotes("\n$notes\n");
//$package->addGlobalReplacement('package-info', '@package_version@', 'version');
$package->generateContents();

if (isset($_GET['make']) || (isset($_SERVER['argv']) && @$_SERVER['argv'][1] == 'make')) {
    $package->writePackageFile();
} else {
    $package->debugPackageFile();
}

?>
