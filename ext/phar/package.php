<?php

$notes = '
 * implement ability connect a phar file \'phar://whatever\' to a directory. That way all
   access to that phar archive are directed to the extracted directory. This
   allows to have the installed files and the archieve keep the same includes.
   [Marcus]
 * implement SHA-2 (256, 512) support [Marcus]
 * implement setSignatureAlgorithm() and Phar::MD5 Phar::SHA1 Phar::SHA256 Phar::SHA512 Phar::PGP to
   choose the kind of signature to use (PGP falls back to SHA1) [Greg]
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
    ),
    'exceptions'        => array(
         'CREDITS'            => 'doc',
         'EXPERIMENTAL'       => 'doc',
         'LICENSE'            => 'doc',
         'Makefile.frag'      => 'src',
         'phar_path_check.re' => 'src',
         'TODO'               => 'doc',
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
