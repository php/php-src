--TEST--
PEAR_Installer test #5: test directory tracking/deletion of installation
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

$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' . DIRECTORY_SEPARATOR . 'package.xml');
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' . DIRECTORY_SEPARATOR . 'package2.xml');
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
var_dump($reg->listPackages());
$info = $reg->packageInfo('pkg1');
echo "pkg1 dir tree contains test/? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test'])) ? "yes\n" : "no\n";
echo "pkg1 dir tree contains test/multiplepackages? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'multiplepackages'])) ? "yes\n" : "no\n";
echo "pkg1 dir tree contains test/pkg1? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'pkg1'])) ? "yes\n" : "no\n";

$info = $reg->packageInfo('pkg2');
echo "pkg2 dir tree contains test/? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test'])) ? "yes\n" : "no\n";
echo "pkg2 dir tree contains test/multiplepackages? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'multiplepackages'])) ? "yes\n" : "no\n";
echo "pkg2 dir tree contains test/nestedroot? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'])) ? "yes\n" : "no\n";
echo "pkg2 dir tree contains test/nestedroot/emptydir? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'
    . DIRECTORY_SEPARATOR . 'emptydir'])) ? "yes\n" : "no\n";
echo "pkg2 dir tree contains test/nestedroot/emptydir/nesteddir? ";
echo (isset($info['filelist']['dirtree'][$temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'
    . DIRECTORY_SEPARATOR . 'emptydir' . DIRECTORY_SEPARATOR . 'nesteddir'])) ? "yes\n" : "no\n";

echo "After uninstall of pkg1:\n";
$installer->uninstall('pkg1');
echo "test/ exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test')) ? "yes\n" : "no\n";
echo "test/multiplepackages exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'multiplepackages')) ? "yes\n" : "no\n";
echo "test/pkg1 exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'pkg1')) ? "yes\n" : "no\n";
echo "test/nestedroot exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot')) ? "yes\n" : "no\n";
echo "test/nestedroot/emptydir exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'
    . DIRECTORY_SEPARATOR . 'emptydir')) ? "yes\n" : "no\n";
echo "test/nestedroot/emptydir/nesteddir exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'
    . DIRECTORY_SEPARATOR . 'emptydir'. DIRECTORY_SEPARATOR . 'nesteddir')) ? "yes\n" : "no\n";
var_dump($reg->listPackages());

echo "After uninstall of pkg2:\n";
$installer->uninstall('pkg2');
echo "test/ exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test')) ? "yes\n" : "no\n";
echo "test/multiplepackages exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'multiplepackages')) ? "yes\n" : "no\n";
echo "test/pkg1 exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'pkg1')) ? "yes\n" : "no\n";
echo "test/nestedroot exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot')) ? "yes\n" : "no\n";
echo "test/nestedroot/emptydir exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'
    . DIRECTORY_SEPARATOR . 'emptydir')) ? "yes\n" : "no\n";
echo "test/nestedroot/emptydir/nesteddir exists? ";
echo (is_dir($temp_path . DIRECTORY_SEPARATOR . 'php'
    . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'nestedroot'
    . DIRECTORY_SEPARATOR . 'emptydir'. DIRECTORY_SEPARATOR . 'nesteddir')) ? "yes\n" : "no\n";
var_dump($reg->listPackages());

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
array(2) {
  [0]=>
  string(4) "pkg1"
  [1]=>
  string(4) "pkg2"
}
pkg1 dir tree contains test/? yes
pkg1 dir tree contains test/multiplepackages? yes
pkg1 dir tree contains test/pkg1? yes
pkg2 dir tree contains test/? yes
pkg2 dir tree contains test/multiplepackages? yes
pkg2 dir tree contains test/nestedroot? yes
pkg2 dir tree contains test/nestedroot/emptydir? yes
pkg2 dir tree contains test/nestedroot/emptydir/nesteddir? yes
After uninstall of pkg1:
test/ exists? yes
test/multiplepackages exists? yes
test/pkg1 exists? no
test/nestedroot exists? yes
test/nestedroot/emptydir exists? yes
test/nestedroot/emptydir/nesteddir exists? yes
array(1) {
  [0]=>
  string(4) "pkg2"
}
After uninstall of pkg2:
test/ exists? no
test/multiplepackages exists? no
test/pkg1 exists? no
test/nestedroot exists? no
test/nestedroot/emptydir exists? no
test/nestedroot/emptydir/nesteddir exists? no
array(0) {
}