--TEST--
PEAR_Installer test #4: PEAR_Installer::install()
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php
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
// make the fake configuration - we'll use one of these and it should work
$config = serialize(array('master_server' => 'pear.php.net',
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
require_once "PEAR/Installer.php";

// no UI is needed for these tests
$ui = false;
$installer = new PEAR_Installer($ui);
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

echo "Test package.xml direct install:\n";
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'package.xml');
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
var_dump($reg->listPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";
echo "After uninstall:\n";
$installer->uninstall('pkg6');
var_dump($reg->listPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";
echo "goompness exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'))
    ? "yes\n" : "no\n";

echo "Test .tgz install:\n";
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-1.1.tgz');
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
var_dump($reg->listPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";
echo "After uninstall:\n";
$installer->uninstall('pkg6');
var_dump($reg->listPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";
echo "goompness exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'))
    ? "yes\n" : "no\n";

echo "Test invalid .tgz install:\n";
$error_to_catch = 'unable to unpack ' . dirname(__FILE__) . DIRECTORY_SEPARATOR .
    'test-pkg6' . DIRECTORY_SEPARATOR . 'invalidtgz.tgz';
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'invalidtgz.tgz');
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
var_dump($reg->listPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";

echo "Test missing package.xml in .tgz install:\n";
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'nopackagexml.tgz');
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
var_dump($reg->listPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";

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
Test package.xml direct install:
array(1) {
  [0]=>
  string(4) "pkg6"
}
zoorb.php exists? yes
goompness/Mopreeb.php exists? yes
goompness/oggbrzitzkee.php exists? yes
After uninstall:
array(0) {
}
zoorb.php exists? no
goompness/Mopreeb.php exists? no
goompness/oggbrzitzkee.php exists? no
goompness exists? no
Test .tgz install:
array(1) {
  [0]=>
  string(4) "pkg6"
}
zoorb.php exists? yes
goompness/Mopreeb.php exists? yes
goompness/oggbrzitzkee.php exists? yes
After uninstall:
array(0) {
}
zoorb.php exists? no
goompness/Mopreeb.php exists? no
goompness/oggbrzitzkee.php exists? no
goompness exists? no
Test invalid .tgz install:
Caught error: Invalid checksum for file "<?xml version="1.0" encoding="ISO-8859-1" ?>
    <!DOCTYPE package SYSTEM "http://pear.php.net/dtd/" : 37649 calculated, 0 expected
Caught expected error
array(0) {
}
zoorb.php exists? no
goompness/Mopreeb.php exists? no
goompness/oggbrzitzkee.php exists? no
Test missing package.xml in .tgz install:
warning : you are using an archive with an old format
Caught error: no package.xml file after extracting the archive
array(0) {
}
zoorb.php exists? no
goompness/Mopreeb.php exists? no
goompness/oggbrzitzkee.php exists? no
