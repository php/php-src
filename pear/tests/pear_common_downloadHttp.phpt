--TEST--
PEAR_Common::downloadHttp test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php
mkdir($temp_path = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testDownloadHttp');
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

putenv('PHP_PEAR_SYSCONF_DIR=' . $temp_path);
$home = getenv('HOME');
if (!empty($home)) {
    // for PEAR_Config initialization
    putenv('HOME="'.$temp_path);
}

require_once "PEAR/Common.php";

$common = &new PEAR_Common;

PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'catchit');

function catchit($err)
{
    echo "Caught error: " . $err->getMessage() . "\n";
}

echo "Test static:\n";
echo "Simple: ";
PEAR_Common::downloadHttp('http://test.pear.php.net/testdownload.tgz', $ui, $temp_path);
$firstone = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testdownload.tgz'));
$secondone = implode('', file($temp_path . DIRECTORY_SEPARATOR . 'testdownload.tgz'));
echo ($firstone == $secondone) ? "passed\n" : "failed\n";
cleanall($temp_path);
echo "Simple fail:\n";
PEAR_Common::downloadHttp('http://test.poop.php.net/stuff.tgz', $ui, $temp_path);

// ------------------------------------------------------------------------- //

function cleanall($dir)
{
    $dp = opendir($dir);
    while ($ent = readdir($dp)) {
        if ($ent == '.' || $ent == '..') {
            continue;
        }
        if (is_dir($ent)) {
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
Test static:
Simple: passed
Simple fail:
Caught error: Connection to `test.poop.php.net:80' failed: The operation completed successfully.