--TEST--
PEAR_Installer test extractDownloadFileName, checkDeps
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

require_once "PEAR/Installer.php";
require_once 'PEAR/ChannelFile.php';

$temp_path = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testinstallertemp';
if (!is_dir($temp_path)) {
    mkdir($temp_path);
}
touch($temp_path . DIRECTORY_SEPARATOR . 'user.conf');
// no UI is needed for these tests
$ui = false;
$config = new PEAR_Config($temp_path . DIRECTORY_SEPARATOR . 'user.conf');
$config->set('php_dir', dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testinstallertemp');
$reg = &new PEAR_Registry($config->get('php_dir'));
$chan = new PEAR_ChannelFile;
$chan->setName('oddball');
$chan->setServer('example.com');
$chan->setSummary('oddball');
$chan->setPackageNameRegex('[a-z][a-z0-9]*(?:\.[a-z0-9]+)*');
$reg->addChannel($chan);
$installer = new PEAR_Installer($ui);
$installer->config = &$config;
echo "test extractDownloadFileName:\n";
echo 'existing file: ';
echo ($temp_path . DIRECTORY_SEPARATOR . 'user.conf' ==
     $installer->extractDownloadFileName($temp_path . DIRECTORY_SEPARATOR . 'user.conf',
    $ui)) ? "yes\n" : "no\n";
var_dump($ui);
echo 'invalid match: ';
echo $installer->extractDownloadFileName('27',
    $ui);
echo "\n";
var_dump($ui);
echo 'valid match, no version: ';
echo $installer->extractDownloadFileName('Testpackage', $ui);
echo "\n";
var_dump($ui);
echo 'invalid match, has invalid version: ';
echo $installer->extractDownloadFileName('Testpackage-##', $ui);
echo "\n";
var_dump($ui);
echo 'valid match, has version: ';
echo $installer->extractDownloadFileName('Testpackage-1.2', $ui);
echo "\n";
var_dump($ui);
echo "valid match, has unknown channel:\n";
var_dump($installer->extractDownloadFileName('Unknown::Testpackage', $ui));
var_dump($ui);
echo "invalid match, has known channel, invalid package name:\n";
var_dump($installer->extractDownloadFileName('Oddball::Testpackage', $ui));
var_dump($ui);
echo "valid match, has known channel, valid package name:\n";
var_dump($installer->extractDownloadFileName('Oddball::testpackage.third3', $ui));
var_dump($ui);
echo "valid match, has known channel, valid package name, version:\n";
var_dump($installer->extractDownloadFileName('Oddball::testpackage.third3-54.7354', $ui));
var_dump($ui);

echo "\ntest checkDeps 1:\n";
$fakerel = array('release_deps' =>
array(
    array(
        'type' => 'pkg',
        'rel '=> 'has',
        'name' => 'foo',
        'optional' => 'yes'
    ),
    array(
        'type' => 'pkg',
        'rel '=> 'ge',
        'version' => '1.6',
        'name' => 'bar',
    ),
));
$res = '';
var_dump($installer->checkDeps($fakerel, $res));
var_dump($res);

echo "\ntest checkDeps 1.1:\n";
$fakerel = array('release_deps' =>
array(
    array(
        'type' => 'pkg',
        'rel '=> 'has',
        'name' => 'foo',
        'optional' => 'yes',
        'channel' => 'grob',
    ),
    array(
        'type' => 'pkg',
        'rel '=> 'ge',
        'version' => '1.6',
        'name' => 'bar',
        'channel' => 'prego',
    ),
));
$res = '';
var_dump($installer->checkDeps($fakerel, $res));
var_dump($res);
$fakerel = array('release_deps' =>
array(
    array(
        'type' => 'pkg',
        'rel '=> 'has',
        'name' => 'foo',
        'optional' => 'yes'
    ),
));
echo "\ntest checkDeps 2:\n";
$res = '';
var_dump($installer->checkDeps($fakerel, $res));
var_dump($res);
$fakerel = array('release_deps' =>
array(
    array(
        'type' => 'pkg',
        'rel '=> 'has',
        'name' => 'foo',
        'optional' => 'yes',
        'channel' => 'purple',
    ),
));
echo "\ntest checkDeps 2.1:\n";
$res = '';
var_dump($installer->checkDeps($fakerel, $res));
var_dump($res);
cleanall($temp_path);
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
--EXPECT--
test extractDownloadFileName:
existing file: yes
bool(false)
invalid match: 27
NULL
valid match, no version: Testpackage
NULL
invalid match, has invalid version: Testpackage-##
NULL
valid match, has version: Testpackage
string(3) "1.2"
valid match, has unknown channel:
array(2) {
  ["channel"]=>
  string(7) "Unknown"
  ["package"]=>
  string(11) "Testpackage"
}
NULL
invalid match, has known channel, invalid package name:
string(20) "Oddball::Testpackage"
NULL
valid match, has known channel, valid package name:
array(2) {
  ["channel"]=>
  string(7) "Oddball"
  ["package"]=>
  string(18) "testpackage.third3"
}
NULL
valid match, has known channel, valid package name, version:
array(2) {
  ["channel"]=>
  string(7) "Oddball"
  ["package"]=>
  string(18) "testpackage.third3"
}
string(7) "54.7354"

test checkDeps 1:
bool(true)
string(29) "
requires package `pear::bar'"

test checkDeps 1.1:
bool(true)
string(30) "
requires package `prego::bar'"

test checkDeps 2:
bool(false)
string(83) "Optional dependencies:
package `pear::foo' is recommended to utilize some features."

test checkDeps 2.1:
bool(false)
string(85) "Optional dependencies:
package `purple::foo' is recommended to utilize some features."