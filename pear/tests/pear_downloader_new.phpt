--TEST--
PEAR_Downloader::download() test
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

echo "Test simple direct url download:\n";

$config = &PEAR_Config::singleton();
$packages = array("http://$server/get/pkg6-1.1.tgz");
$options = array();
$installer = &new PEAR_Downloader($ui, $options, $config);
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());

echo "File exists? ";
echo (is_file($installpackages[0]['file'])) ? "yes\n" : "no\n";
echo "File is the same? ";
$good = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-1.1.tgz'));
$dled = implode('', file($installpackages[0]['file']));
echo ($good == $dled) ? "yes\n" : "no\n";
unlink($installpackages[0]['file']);
unset($installpackages[0]['file']);
var_dump($installpackages);

echo "Test simple package name download:\n";
$installpackages = $errors = array();
$packages = array('pkg6');
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());

echo "File exists? ";
echo (is_file($installpackages[0]['file'])) ? "yes\n" : "no\n";
echo "File is the same? ";
$good = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-1.1.tgz'));
$dled = implode('', file($installpackages[0]['file']));
echo ($good == $dled) ? "yes\n" : "no\n";
unlink($installpackages[0]['file']);
unset($installpackages[0]['file']);
var_dump($installpackages);

echo "Test package name with version download:\n";
$installpackages = $errors = array();
$packages = array('pkg6-1.1');
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());

echo "File exists? ";
echo (is_file($installpackages[0]['file'])) ? "yes\n" : "no\n";
echo "File is the same? ";
$good = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-1.1.tgz'));
$dled = implode('', file($installpackages[0]['file']));
echo ($good == $dled) ? "yes\n" : "no\n";
unlink($installpackages[0]['file']);

echo "Test package name with state stable download:\n";
$installpackages = $errors = array();
$packages = array('pkg6-stable');
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());

echo "File exists? ";
echo (is_file($installpackages[0]['file'])) ? "yes\n" : "no\n";
echo "File is the same? ";
$good = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-1.1.tgz'));
$dled = implode('', file($installpackages[0]['file']));
echo ($good == $dled) ? "yes\n" : "no\n";
unlink($installpackages[0]['file']);

echo "Test package name with state beta download:\n";
$installpackages = $errors = array();
$packages = array('pkg6-beta');
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());

echo "File exists? ";
echo (is_file($installpackages[0]['file'])) ? "yes\n" : "no\n";
echo "File is the same? ";
$good = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-2.0b1.tgz'));
$dled = implode('', file($installpackages[0]['file']));
echo ($good == $dled) ? "yes\n" : "no\n";
unlink($installpackages[0]['file']);

echo "================\nTest preferred_state = beta\n";
$installer->configSet('preferred_state', 'beta');

echo "Test simple package name download:\n";
$installpackages = $errors = array();
$packages = array('pkg6');
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());

echo "File exists? ";
echo (is_file($installpackages[0]['file'])) ? "yes\n" : "no\n";
echo "File is the same? ";
$good = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test-pkg6' . DIRECTORY_SEPARATOR . 'pkg6-2.0b1.tgz'));
$dled = implode('', file($installpackages[0]['file']));
echo ($good == $dled) ? "yes\n" : "no\n";
unlink($installpackages[0]['file']);
unset($installpackages[0]['file']);

echo "\n================Test --alldeps\nTest preferred_state = stable\n";
$installer->configSet('preferred_state', 'stable');

$installer = &new PEAR_Downloader($ui, array('alldeps' => true), $config);
$packages = array("http://$server/get/pkg1-1.1.tgz");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}
$packages = array("pkg1");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}

echo "Test preferred_state = beta:\n";
$installer->configSet('preferred_state', 'beta');

$packages = array("pkg1");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}

echo "Test preferred_state = alpha:\n";
$installer->configSet('preferred_state', 'alpha');

$packages = array("pkg1");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}
echo "\n================Test --onlyreqdeps\nTest preferred_state = stable\n";
$config->set('preferred_state', 'stable');

$installer = &new PEAR_Downloader($ui, array('onlyreqdeps' => true), $config);
$packages = array("http://$server/get/pkg1-1.1.tgz");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}
$packages = array("pkg1");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}

echo "Test preferred_state = beta:\n";
$installer->configSet('preferred_state', 'beta');

$packages = array("pkg1");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}

echo "Test preferred_state = alpha:\n";
$installer->configSet('preferred_state', 'alpha');

$packages = array("pkg1");
$a = $installer->download($packages);
$installpackages = $installer->getDownloadedPackages();
var_dump($a, $installer->getErrorMsgs());
echo "Packages downloaded and version:\n";
foreach ($installpackages as $package) {
    echo $package['pkg'] . '-' . $package['info']['version'] . "\n";
}


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
Test simple direct url download:
NULL
array(0) {
}
File exists? yes
File is the same? yes
array(1) {
  [0]=>
  array(2) {
    ["pkg"]=>
    string(4) "pkg6"
    ["info"]=>
    array(11) {
      ["provides"]=>
      array(0) {
      }
      ["filelist"]=>
      &array(3) {
        ["zoorb.php"]=>
        array(2) {
          ["role"]=>
          string(3) "php"
          ["baseinstalldir"]=>
          string(5) "groob"
        }
        ["goompness\oggbrzitzkee.php"]=>
        array(2) {
          ["role"]=>
          string(3) "php"
          ["baseinstalldir"]=>
          string(5) "groob"
        }
        ["goompness\Mopreeb.php"]=>
        array(2) {
          ["role"]=>
          string(3) "php"
          ["baseinstalldir"]=>
          string(5) "groob"
        }
      }
      ["package"]=>
      string(4) "pkg6"
      ["summary"]=>
      string(32) "required test for PEAR_Installer"
      ["description"]=>
      string(12) "fake package"
      ["maintainers"]=>
      array(1) {
        [0]=>
        &array(4) {
          ["handle"]=>
          string(8) "fakeuser"
          ["name"]=>
          string(9) "Joe Shmoe"
          ["email"]=>
          string(18) "nobody@example.com"
          ["role"]=>
          string(4) "lead"
        }
      }
      ["version"]=>
      string(3) "1.1"
      ["release_date"]=>
      string(10) "2003-09-09"
      ["release_license"]=>
      string(11) "PHP License"
      ["release_state"]=>
      string(6) "stable"
      ["release_notes"]=>
      string(24) "required dependency test"
    }
  }
}
Test simple package name download:
NULL
array(0) {
}
File exists? yes
File is the same? yes
array(1) {
  [0]=>
  array(2) {
    ["pkg"]=>
    string(4) "pkg6"
    ["info"]=>
    array(11) {
      ["provides"]=>
      array(0) {
      }
      ["filelist"]=>
      &array(3) {
        ["zoorb.php"]=>
        array(2) {
          ["role"]=>
          string(3) "php"
          ["baseinstalldir"]=>
          string(5) "groob"
        }
        ["goompness\oggbrzitzkee.php"]=>
        array(2) {
          ["role"]=>
          string(3) "php"
          ["baseinstalldir"]=>
          string(5) "groob"
        }
        ["goompness\Mopreeb.php"]=>
        array(2) {
          ["role"]=>
          string(3) "php"
          ["baseinstalldir"]=>
          string(5) "groob"
        }
      }
      ["package"]=>
      string(4) "pkg6"
      ["summary"]=>
      string(32) "required test for PEAR_Installer"
      ["description"]=>
      string(12) "fake package"
      ["maintainers"]=>
      array(1) {
        [0]=>
        &array(4) {
          ["handle"]=>
          string(8) "fakeuser"
          ["name"]=>
          string(9) "Joe Shmoe"
          ["email"]=>
          string(18) "nobody@example.com"
          ["role"]=>
          string(4) "lead"
        }
      }
      ["version"]=>
      string(3) "1.1"
      ["release_date"]=>
      string(10) "2003-09-09"
      ["release_license"]=>
      string(11) "PHP License"
      ["release_state"]=>
      string(6) "stable"
      ["release_notes"]=>
      string(24) "required dependency test"
    }
  }
}
Test package name with version download:
NULL
array(0) {
}
File exists? yes
File is the same? yes
Test package name with state stable download:
NULL
array(0) {
}
File exists? yes
File is the same? yes
Test package name with state beta download:
NULL
array(0) {
}
File exists? yes
File is the same? yes
================
Test preferred_state = beta
Test simple package name download:
NULL
array(0) {
}
File exists? yes
File is the same? yes

================Test --alldeps
Test preferred_state = stable
NULL
array(0) {
}
Packages downloaded and version:
pkg1-1.1
pkg2-1.1
pkg3-1.1
pkg4-1.1
pkg5-1.1
pkg6-1.1
NULL
array(0) {
}
Packages downloaded and version:
pkg1-1.1
pkg2-1.1
pkg3-1.1
pkg4-1.1
pkg5-1.1
pkg6-1.1
Test preferred_state = beta:
NULL
array(0) {
}
Packages downloaded and version:
pkg1-2.0b1
pkg2-1.1
pkg3-1.1
pkg4-1.1
pkg5-1.1
pkg6-2.0b1
Test preferred_state = alpha:
NULL
array(0) {
}
Packages downloaded and version:
pkg1-2.0b1
pkg2-1.1
pkg3-1.4
pkg4AndAHalf-1.3
pkg4-1.1
pkg5-1.1
pkg6-2.0b1

================Test --onlyreqdeps
Test preferred_state = stable
NULL
array(0) {
}
Packages downloaded and version:
pkg1-1.1
pkg2-1.1
pkg3-1.1
pkg4-1.1
pkg5-1.1
pkg6-1.1
NULL
array(0) {
}
Packages downloaded and version:
pkg1-1.1
pkg2-1.1
pkg3-1.1
pkg4-1.1
pkg5-1.1
pkg6-1.1
Test preferred_state = beta:
NULL
array(0) {
}
Packages downloaded and version:
pkg1-2.0b1
pkg2-1.1
pkg3-1.1
pkg4-1.1
pkg5-1.1
pkg6-2.0b1
Test preferred_state = alpha:
skipping Package 'pkg3' optional dependency 'pkg4AndAHalf'
NULL
array(0) {
}
Packages downloaded and version:
pkg1-2.0b1
pkg2-1.1
pkg3-1.4
pkg4-1.1
pkg5-1.1
pkg6-2.0b1