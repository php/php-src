--TEST--
PEAR_Installer test #4: PEAR_Installer::install() with channels
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
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob');
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
    'default_channel' => 'pear',
    'php_dir' => $temp_path . DIRECTORY_SEPARATOR . 'php',
    'ext_dir' => $temp_path . DIRECTORY_SEPARATOR . 'ext',
    'data_dir' => $temp_path . DIRECTORY_SEPARATOR . 'data',
    'doc_dir' => $temp_path . DIRECTORY_SEPARATOR . 'doc',
    'test_dir' => $temp_path . DIRECTORY_SEPARATOR . 'test',
    'bin_dir' => $temp_path . DIRECTORY_SEPARATOR . 'bin',
    '__channels' => array(
        'frob' => array(
            'php_dir' => $temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob',
    ))));
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
require_once 'PEAR/ChannelFile.php';
require_once 'PEAR/Registry.php';
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
$chan = new PEAR_ChannelFile;
$chan->setName('frob');
$chan->setSummary('test');
$chan->setServer('test');
$reg->addChannel($chan);
$chan->setName('groob');
$reg->addChannel($chan);
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
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'package2.xml');
$reg = &new PEAR_Registry($temp_path . DIRECTORY_SEPARATOR . 'php');
var_dump($reg->listAllPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";
echo "goompness/test.dat exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'data' . DIRECTORY_SEPARATOR . 'pkg6'
    . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'test.dat')) ? "yes\n" : "no\n";

echo "Test conflicting files:\n";
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'conflictpackage2.xml');

echo "Test unknown channel:\n";
$installer->install(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'package2_invalid.xml');

echo "After uninstall:\n";
$installer->uninstall('frob::pkg6');
var_dump($reg->listAllPackages());
echo "zoorb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'zoorb.php')) ? "yes\n" : "no\n";
echo "goompness/Mopreeb.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'Mopreeb.php')) ? "yes\n" : "no\n";
echo "goompness/oggbrzitzkee.php exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'frob'
    . DIRECTORY_SEPARATOR . 'groob' . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'oggbrzitzkee.php')) ? "yes\n" : "no\n";
echo "goompness/test.dat exists? ";
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'data' . DIRECTORY_SEPARATOR . 'pkg6'
    . DIRECTORY_SEPARATOR . 'goompness'
    . DIRECTORY_SEPARATOR . 'test.dat')) ? "yes\n" : "no\n";

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
array(3) {
  ["frob"]=>
  array(1) {
    [0]=>
    string(4) "pkg6"
  }
  ["groob"]=>
  array(0) {
  }
  ["pear"]=>
  array(0) {
  }
}
zoorb.php exists? yes
goompness/Mopreeb.php exists? yes
goompness/oggbrzitzkee.php exists? yes
goompness/test.dat exists? yes
Test conflicting files:
Caught error: groob::pkg6conflict: conflicting files found:
groob\goompness\oggbrzitzkee.php (frob::pkg6)
     groob\goompness\Mopreeb.php (frob::pkg6)
        groob\goompness\test.dat (frob::pkg6)

Test unknown channel:
Error: Unknown channel, "gorp"
Caught error: Installation failed: invalid package file, use option force to install anyway
After uninstall:
array(3) {
  ["frob"]=>
  array(0) {
  }
  ["groob"]=>
  array(0) {
  }
  ["pear"]=>
  array(0) {
  }
}
zoorb.php exists? no
goompness/Mopreeb.php exists? no
goompness/oggbrzitzkee.php exists? no
goompness/test.dat exists? no
