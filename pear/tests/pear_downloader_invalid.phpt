--TEST--
PEAR_Downloader::download() invalid values test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

$server = 'pear.Chiara';
//$server = 'test.pear.php.net';
$temp_path = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testinstallertemp';
if (!is_dir($temp_path)) {
    mkdir($temp_path);
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'php')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'php');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'data')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'data');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'doc')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'doc');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'test')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'test');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'ext')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'ext');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'script')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'script');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'tmp')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'tmp');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'bin')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'bin');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'cache')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'cache');
}
// make the fake configuration - we'll use one of these and it should work
$config = serialize(array('master_server' => $server,
    'preferred_state' => 'stable',
    'cache_dir' => $temp_path . DIRECTORY_SEPARATOR . 'cache',
    'php_dir' => $temp_path . DIRECTORY_SEPARATOR . 'php',
    'ext_dir' => $temp_path . DIRECTORY_SEPARATOR . 'ext',
    'data_dir' => $temp_path . DIRECTORY_SEPARATOR . 'data',
    'doc_dir' => $temp_path . DIRECTORY_SEPARATOR . 'doc',
    'test_dir' => $temp_path . DIRECTORY_SEPARATOR . 'test',
    'bin_dir' => $temp_path . DIRECTORY_SEPARATOR . 'bin',));
touch($temp_path . DIRECTORY_SEPARATOR . 'pear.conf');
$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'pear.conf', 'w');
fwrite($fp, $config);
fclose($fp);
touch($temp_path . DIRECTORY_SEPARATOR . 'pear.ini');
$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'pear.ini', 'w');
fwrite($fp, $config);
fclose($fp);

putenv('PHP_PEAR_SYSCONF_DIR='.$temp_path);
$home = getenv('HOME');
if (!empty($home)) {
    // for PEAR_Config initialization
    putenv('HOME="'.$temp_path);
}
require_once "PEAR/Downloader.php";

// no UI is needed for these tests
$ui = false;
$curdir = getcwd();
chdir(dirname(__FILE__));

PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'catchit');
$error_to_catch = false;
function catchit($err)
{
    global $error_to_catch;
    if ($error_to_catch) {
        if ($err->getMessage() == $error_to_catch) {
            $error_to_catch = false;
            echo "Caught expected error\n";
            return;
        }
    }
    echo "Caught error: " . $err->getMessage() . "\n";
}

$config = &PEAR_Config::singleton();
$options = array();
$installer = &new PEAR_Downloader($ui, $options, $config);

echo "-=-=-=-=-=-=-=-=- Failure Tests -=-=-=-=-=-=-=-=-=-=-\n";

echo "Test invalid package name:\n";
$packages = array("/invalid+packagename");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages);

echo "Test download of a package with no releases:\n";
$packages = array("noreleases");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages);

echo "Test download of a non-existing package version:\n";
$packages = array("pkg1-1976.9.2");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages);

echo "Test download of a non-existing package release state:\n";
$packages = array("pkg1-snapshot");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages);

echo "Test download of invalid release state:\n";
$packages = array("pkg1-burgerking");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages);

$installer->configSet('preferred_state', 'stable');

echo "Test automatic version resolution (stable):\n";
$packages = array("stabilitytoolow");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages);

echo "Test automatic version resolution (stable) with --force:\n";
$packages = array("stabilitytoolow");
$installer->setOptions(array('force' => true));
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages[0]['info']['version']);

$installer->configSet('preferred_state', 'beta');

echo "Test automatic version resolution (beta):\n";
$packages = array("stabilitytoolow");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages[0]['info']['version']);

$installer->configSet('preferred_state', 'alpha');

echo "Test automatic version resolution (alpha):\n";
$packages = array("stabilitytoolow");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages[0]['info']['version']);

$installer->configSet('preferred_state', 'devel');

echo "Test automatic version resolution (devel):\n";
$packages = array("stabilitytoolow");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $installpackages[0]['info']['version']);

echo "Test download attempt if a version is already installed:\n";

require_once 'PEAR/Installer.php';
$install = &new PEAR_Installer($ui);
$installer->setOptions(array());

$installer->download(array('pkg6'));
$pkgs = $installer->getDownloadedPackages();
$install->install($pkgs[0]['file']);
$installer->download(array('pkg6'));
var_dump(get_class($a), $installer->getErrorMsgs());

echo "Test download attempt if a version is already installed with --force:\n";

$installer->setOptions(array('force' => true));

$installer->download(array('pkg6'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs(), $pkgs[0]['info']['version']);

echo "Test download attempt if a version is already installed with upgrade, same version:\n";

$installer->setOptions(array('upgrade' => true));

$installer->download(array('pkg6'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs());

echo "Test download attempt if a version is already installed with upgrade, lesser version:\n";

$installer->setOptions(array('upgrade' => true));

$installer->download(array('pkg6-1.1'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs());

echo "Test download attempt with --alldeps, but dependency has no releases:\n";

$installer->setOptions(array('alldeps' => true));

$installer->download(array('depnoreleases'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs());

echo "Test download attempt with --onlyreqdeps, but dependency has no releases:\n";

$installer->setOptions(array('onlyreqdeps' => true));

$installer->download(array('depnoreleases'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs());

$installer->configSet('preferred_state', 'stable');
echo "Test download attempt with --alldeps, but dependency is too unstable:\n";
$installer->setOptions(array('alldeps' => true));

$installer->download(array('depunstable'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs());

echo "Test download attempt with --onlyreqdeps, but dependency is too unstable:\n";

$installer->setOptions(array('onlyreqdeps' => true));

$installer->download(array('depunstable'));
$pkgs = $installer->getDownloadedPackages();
var_dump(get_class($a), $installer->getErrorMsgs());

chdir($curdir);
cleanall($temp_path);
// ------------------------------------------------------------------------- //

function cleanall($dir)
{
    $dp = opendir($dir);
    while ($ent = readdir($dp)) {
        if ($ent == '.' || $ent == '..') {
            continue;
        }
        if (is_dir($dir . DIRECTORY_SEPARATOR . $ent)) {
            cleanall($dir . DIRECTORY_SEPARATOR . $ent);
        } else {
            unlink($dir . DIRECTORY_SEPARATOR . $ent);
        }
    }
    closedir($dp);
    rmdir($dir);
}
?>
--GET--
--POST--
--EXPECT--
-=-=-=-=-=-=-=-=- Failure Tests -=-=-=-=-=-=-=-=-=-=-
Test invalid package name:
Caught error: Package name '/invalid+packagename' not valid
string(10) "pear_error"
array(0) {
}
array(0) {
}
Test download of a package with no releases:
Caught error: No releases found for package 'noreleases'
string(10) "pear_error"
array(0) {
}
array(0) {
}
Test download of a non-existing package version:
Caught error: No release with version '1976.9.2' found for 'pkg1'
string(10) "pear_error"
array(0) {
}
array(0) {
}
Test download of a non-existing package release state:
Caught error: No release with state 'snapshot' found for 'pkg1'
string(10) "pear_error"
array(0) {
}
array(0) {
}
Test download of invalid release state:
Caught error: Invalid postfix '-burgerking', be sure to pass a valid PEAR version number or release state
string(10) "pear_error"
array(0) {
}
array(0) {
}
Test automatic version resolution (stable):
Caught error: No release with state equal to: 'stable' found for 'stabilitytoolow'
string(10) "pear_error"
array(0) {
}
array(0) {
}
Test automatic version resolution (stable) with --force:
Warning: stabilitytoolow is state 'devel' which is less stable than state 'stable'
bool(false)
array(0) {
}
string(6) "3.0dev"
Test automatic version resolution (beta):
bool(false)
array(0) {
}
string(5) "1.0b1"
Test automatic version resolution (alpha):
bool(false)
array(0) {
}
string(5) "2.0a1"
Test automatic version resolution (devel):
bool(false)
array(0) {
}
string(6) "3.0dev"
Test download attempt if a version is already installed:
Package 'pkg6' already installed, skipping
bool(false)
array(0) {
}
Test download attempt if a version is already installed with --force:
bool(false)
array(0) {
}
string(5) "2.0b1"
Test download attempt if a version is already installed with upgrade, same version:
Package 'pkg6-2.0b1' already installed, skipping
bool(false)
array(0) {
}
Test download attempt if a version is already installed with upgrade, lesser version:
Package 'pkg6' version '2.0b1'  is installed and 2.0b1 is > requested '1.1', skipping
bool(false)
array(0) {
}
Test download attempt with --alldeps, but dependency has no releases:
bool(false)
array(1) {
  [0]=>
  string(63) "Package 'depnoreleases' dependency 'noreleases' has no releases"
}
Test download attempt with --onlyreqdeps, but dependency has no releases:
bool(false)
array(1) {
  [0]=>
  string(63) "Package 'depnoreleases' dependency 'noreleases' has no releases"
}
Test download attempt with --alldeps, but dependency is too unstable:
bool(false)
array(1) {
  [0]=>
  string(91) "Release for 'depunstable' dependency 'stabilitytoolow' has state 'devel', requires 'stable'"
}
Test download attempt with --onlyreqdeps, but dependency is too unstable:
bool(false)
array(1) {
  [0]=>
  string(91) "Release for 'depunstable' dependency 'stabilitytoolow' has state 'devel', requires 'stable'"
}