<?php

$notes = '
BC BREAKING RELEASE
 BC breaks:
 * Phar object Compression API is rewritten.  Use Phar::compress() and decompress(),
   Phar::compressFiles()/decompressFiles() and PharFileInfo->compress()/decompress().
 * phar.extract_list and Phar::getExtractList() are removed

Major feature functionality release
 * new default stub allows running of phar-based phars without phar extension [Greg/Steph]
 * add support for tar-based and zip-based phar archives [Greg]
 * add Phar::isTar(), Phar::isZip(), and Phar::isPhar() [Greg]
 * add Phar::convertToExecutable(), Phar::convertToData() [Greg]
 * add Phar::compress() [Greg]
 * rename Phar::compressAllFiles() to compressFiles(), uncompressAllFiles() to
   decompressFiles() [Greg]
 * conversion to compressed or to other file formats automatically copies the archive
   to a new extension (i.e. ".phar" to ".phar.tar" or ".tar" to ".tar.gz") [Steph]
 * add Phar::webPhar() for running a web-based application unmodified
   directly from a phar archive [Greg]
 * file functions (fopen-based and stat-based) can be instructed to only look for
   relative paths within a phar via Phar::interceptFileFuncs()
 * add PharData class to allow manipulation/creation of non-executable tar and zip archives. [Steph]
   non-executable tar/zip manipulation is allowed even when phar.readonly=1 [Greg]
 * paths with . and .. work (phar://blah.phar/a/../b.php => phar://blah.phar/b.php) [Greg]
 * add support for mkdir()/rmdir() and support for empty directories to phar file format [Greg]
 * add option to compress the entire phar file for phar/tar file format [Greg]
 * implement Phar::isCompressed() returning 0, Phar::GZ or Phar::BZ2 [Greg]
 * implement Phar::copy(string $from, string $to) [Greg]
 * implement Phar::buildFromIterator(Iterator $it[, string $base_directory]) [Greg]
 * implement Phar::mount() for mounting external paths or files to locations inside a phar [Greg]
 * add Phar::delete() [Greg]

Security addition
 * aliases are validated so that they contain no directory separators as intended
 * on conversion to other formats, user-supplied aliases are validated

Changes since 2.0.0a2: many bugfixes, removal of phar.extract_list, compression API refactored,
 conversion API refactored
';

if (!class_exists("Phar") && !extension_loaded("Phar")) {
	die("Extension phar not present");
}
error_reporting(E_ALL & ~E_DEPRECATED);

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
$package->setReleaseStability('beta');
$package->setAPIStability('beta');
$package->setNotes("\n$notes\n");
//$package->addGlobalReplacement('package-info', '@package_version@', 'version');
$package->generateContents();

if (isset($_GET['make']) || (isset($_SERVER['argv']) && @$_SERVER['argv'][1] == 'make')) {
    $package->writePackageFile();
} else {
    $package->debugPackageFile();
}

?>
