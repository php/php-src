--TEST--
PEAR_Installer test #1
--SKIPIF--
skip
--FILE--
<?php

require_once "PEAR/Installer.php";

// no UI is needed for these tests
$ui = false;
$installer = new PEAR_Installer($ui);
$temp_path = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testinstallertemp';
if (!is_dir($temp_path)) {
    mkdir($temp_path);
}
// make the fake configuration
$config = serialize(array('master_server' => 'pear.php.net', 'php_dir' => $temp_path));
touch($temp_path . DIRECTORY_SEPARATOR . 'user.conf');
$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'user.conf', 'w');
fwrite($fp, $config);
fclose($fp);
echo "test extractDownloadFileName:\n";
echo 'existing file: ';
echo $installer->extractDownloadFileName($temp_path . DIRECTORY_SEPARATOR . 'user.conf',
    $ui);
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
// '/^([A-Z][a-zA-Z0-9_]+|[a-z][a-z0-9_]+)(-([.0-9a-zA-Z]+))?$/'
unlink ($temp_path . DIRECTORY_SEPARATOR . 'user.conf');
rmdir($temp_path);
?>
--GET--
--POST--
--EXPECT--
test extractDownloadFileName:
existing file: c:\web pages\chiara\testinstallertemp\user.confbool(false)
invalid match: 27
NULL
valid match, no version: Testpackage
NULL
invalid match, has invalid version: Testpackage-##
NULL
valid match, has version: Testpackage
string(3) "1.2"