--TEST--
PEAR_Installer test #1
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

require_once "PEAR/Installer.php";

$temp_path = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testinstallertemp';
if (!is_dir($temp_path)) {
    mkdir($temp_path);
}
touch($temp_path . DIRECTORY_SEPARATOR . 'user.conf');
// no UI is needed for these tests
$ui = false;
$installer = new PEAR_Installer($ui);
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
unlink($temp_path . DIRECTORY_SEPARATOR . 'user.conf');
rmdir($temp_path);
?>
--GET--
--POST--
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

test checkDeps 1:
bool(true)
string(23) "
requires package `bar'"

test checkDeps 2:
bool(false)
string(77) "Optional dependencies:
package `foo' is recommended to utilize some features."
