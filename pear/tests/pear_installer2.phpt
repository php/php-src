--TEST--
PEAR_Installer test #2
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

echo "test _installFile():\n";
$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installer2.phpt.testfile.php', 'w');
fwrite($fp, 'a');
fclose($fp);
// pretend we just parsed a package.xml
$installer->pkginfo = array('package' => 'Foo');

echo "install as role=\"php\":\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'php'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file ext/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "install as role=\"ext\":\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'ext'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file php/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'ext' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "install as role=\"data\":\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'data'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file data/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'data' . DIRECTORY_SEPARATOR .
    'Foo' . DIRECTORY_SEPARATOR . '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "install as role=\"doc\":\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'doc'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file doc/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'doc' . DIRECTORY_SEPARATOR .
    'Foo' . DIRECTORY_SEPARATOR . '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "install as role=\"test\":\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'test'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file test/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR .
    'Foo' . DIRECTORY_SEPARATOR . '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "install as role=\"script\":\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file bin/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

$installer->rollbackFileTransaction();

echo "install as invalid role=\"klingon\":\n";
$err = $installer->_installFile('installer2.phpt.testfile.php', array('role' => 'klingon'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array());
echo 'returned PEAR_Error: ' . (get_class($err) == 'pear_error' ? "yes\n" : "no\n");
if (is_object($err)) {
    echo 'message: ' . $err->getMessage() . "\n\n";
}
echo 'file bin/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "install non-existent file:\n";
$err = $installer->_installFile('....php', array('role' => 'php'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array());
echo 'returned PEAR_Error: ' . (get_class($err) == 'pear_error' ? "yes\n" : "no\n");
if (is_object($err)) {
    echo 'message: ' . $err->getMessage() . "\n";
}
echo 'file bin/.tmp....php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmp....php') ? "yes\n" : "no\n");

$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installer2.phpt.testfile.php', 'w');
fwrite($fp, '@TEST@ stuff');
fclose($fp);

echo "\ntest valid md5sum:\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script', 'md5sum' => md5('@TEST@ stuff')),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo 'file bin/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

$installer->rollbackFileTransaction();

echo "test invalid md5sum:\n";
$err = $installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script', 'md5sum' => md5('oops stuff')),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array());
echo 'returned PEAR_Error: ' . (get_class($err) == 'pear_error' ? "yes\n" : "no\n");
if (is_object($err)) {
    echo 'message: ' . ($err->getMessage() == 'bad md5sum for file ' . $temp_path . DIRECTORY_SEPARATOR . 'bin' .
    DIRECTORY_SEPARATOR . 'installer2.phpt.testfile.php' ? 'match' : 'no match') . "\n";
}
echo 'file bin/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

echo "test invalid md5sum with --force:\n";
ob_start();
$err = $installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script', 'md5sum' => md5('oops stuff')),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array('force' => true));
$warning = ob_get_contents();
ob_end_clean();
echo 'warning : ';
echo ($warning == 'warning : bad md5sum for file ' . $temp_path . DIRECTORY_SEPARATOR . 'bin' .
    DIRECTORY_SEPARATOR . "installer2.phpt.testfile.php\n" ? "match\n" : "no match\n");
echo 'returned PEAR_Error: ' . (get_class($err) == 'pear_error' ? "yes\n" : "no\n");
if (is_object($err)) {
    echo 'message: ' . ($err->getMessage() == 'bad md5sum for file ' . $temp_path . DIRECTORY_SEPARATOR . 'bin' .
    DIRECTORY_SEPARATOR . 'installer2.phpt.testfile.php' ? 'match' : 'no match') . "\n";
}
echo 'file bin/.tmpinstaller2.phpt.testfile.php exists? => ';
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php') ? "yes\n" : "no\n");

define('PEARINSTALLERTEST2_FAKE_FOO_CONST', 'good');
echo "\ntest replacements:\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'replacements' => array(array('type' => 'php-const', 'from' => '@TEST@', 'to' => 'PEARINSTALLERTEST2_FAKE_FOO_CONST'))),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo "==>test php-const replacement: equals 'good stuff'? => ";
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'))
{
    $a = implode(file($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'), '');
    echo "$a\n";
} else {
    echo "no! file installation failed\n";
}
$installer->rollbackFileTransaction();

echo "==>test invalid php-const replacement:\n";
$err = $installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'replacements' => array(array('type' => 'php-const', 'from' => '@TEST@', 'to' => '%PEARINSTALLERTEST2_FAKE_FOO_CONST'))),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array());
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'))
{
    $a = implode(file($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'), '');
    echo "$a\n";
} else {
    echo "no! file installation failed\n";
}

$installer->rollbackFileTransaction();

var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'replacements' => array(array('type' => 'pear-config', 'from' => '@TEST@', 'to' => 'master_server'))),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo "==>test pear-config replacement: equals 'pear.php.net stuff'? => ";
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'))
{
    $a = implode(file($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'), '');
    echo "$a\n";
} else {
    echo "no! file installation failed\n";
}
$installer->rollbackFileTransaction();

echo "==>test invalid pear-config replacement\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'replacements' => array(array('type' => 'pear-config', 'from' => '@TEST@', 'to' => 'blahblahblah'))),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'))
{
    $a = implode(file($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'), '');
    echo "$a\n";
} else {
    echo "no! file installation failed\n";
}
$installer->rollbackFileTransaction();

var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'replacements' => array(array('type' => 'package-info', 'from' => '@TEST@', 'to' => 'package'))),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo "==>test package-info replacement: equals 'Foo stuff'? => ";
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'))
{
    $a = implode(file($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'), '');
    echo "$a\n";
} else {
    echo "no! file installation failed\n";
}
$installer->rollbackFileTransaction();

echo "==>test invalid package-info replacement:\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'replacements' => array(array('type' => 'package-info', 'from' => '@TEST@', 'to' => 'gronk'))),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'))
{
    $a = implode(file($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpinstaller2.phpt.testfile.php'), '');
    echo "$a\n";
} else {
    echo "no! file installation failed\n";
}
$installer->rollbackFileTransaction();

echo "\ntest install-as:\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'install-as' => 'foobar.php'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo "==>test install as 'foobar.php'.  file exists? ";
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    '.tmpfoobar.php'))
{
    echo "yes\n";
} else {
    echo "no\n";
}
$installer->rollbackFileTransaction();

echo "\ntest baseinstalldir:\n";
var_dump($installer->_installFile('installer2.phpt.testfile.php', array('role' => 'script',
    'baseinstalldir' => 'Foo/Mine'),
    $temp_path . DIRECTORY_SEPARATOR . 'tmp', array()));
echo "==>test baseinstalldir = 'Foo/Mine'.  file exists? ";
if (file_exists($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    'Foo' . DIRECTORY_SEPARATOR . 'Mine' . DIRECTORY_SEPARATOR . '.tmpinstaller2.phpt.testfile.php'))
{
    echo "yes\n";
} else {
    echo "no\n";
}
$installer->rollbackFileTransaction();

rmdir($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    'Foo' . DIRECTORY_SEPARATOR . 'Mine');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'bin' . DIRECTORY_SEPARATOR .
    'Foo');
unlink($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installer2.phpt.testfile.php');
//cleanup
chdir($curdir);
unlink ($temp_path . DIRECTORY_SEPARATOR . 'pear.conf');
unlink ($temp_path . DIRECTORY_SEPARATOR . 'pear.ini');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'php');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'data' . DIRECTORY_SEPARATOR . 'Foo');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'data');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'doc' . DIRECTORY_SEPARATOR . 'Foo');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'doc');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'test' . DIRECTORY_SEPARATOR . 'Foo');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'test');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'script');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'ext');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'tmp');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'bin');
rmdir($temp_path);
?>
--GET--
--POST--
--EXPECT--
test _installFile():
install as role="php":
int(1)
file ext/.tmpinstaller2.phpt.testfile.php exists? => yes
install as role="ext":
int(1)
file php/.tmpinstaller2.phpt.testfile.php exists? => yes
install as role="data":
int(1)
file data/.tmpinstaller2.phpt.testfile.php exists? => yes
install as role="doc":
int(1)
file doc/.tmpinstaller2.phpt.testfile.php exists? => yes
install as role="test":
int(1)
file test/.tmpinstaller2.phpt.testfile.php exists? => yes
install as role="script":
int(1)
file bin/.tmpinstaller2.phpt.testfile.php exists? => yes
install as invalid role="klingon":
returned PEAR_Error: yes
message: Invalid role `klingon' for file installer2.phpt.testfile.php

file bin/.tmpinstaller2.phpt.testfile.php exists? => no
install non-existent file:
returned PEAR_Error: yes
message: file does not exist
file bin/.tmp....php exists? => no

test valid md5sum:
int(1)
file bin/.tmpinstaller2.phpt.testfile.php exists? => yes
test invalid md5sum:
returned PEAR_Error: yes
message: match
file bin/.tmpinstaller2.phpt.testfile.php exists? => no
test invalid md5sum with --force:
warning : match
returned PEAR_Error: no
file bin/.tmpinstaller2.phpt.testfile.php exists? => yes

test replacements:
int(1)
==>test php-const replacement: equals 'good stuff'? => good stuff
==>test invalid php-const replacement:
invalid php-const replacement: %PEARINSTALLERTEST2_FAKE_FOO_CONST
@TEST@ stuff
int(1)
==>test pear-config replacement: equals 'pear.php.net stuff'? => pear.php.net stuff
==>test invalid pear-config replacement
invalid pear-config replacement: blahblahblah
int(1)
@TEST@ stuff
int(1)
==>test package-info replacement: equals 'Foo stuff'? => Foo stuff
==>test invalid package-info replacement:
invalid package-info replacement: gronk
int(1)
@TEST@ stuff

test install-as:
int(1)
==>test install as 'foobar.php'.  file exists? yes

test baseinstalldir:
int(1)
==>test baseinstalldir = 'Foo/Mine'.  file exists? yes

