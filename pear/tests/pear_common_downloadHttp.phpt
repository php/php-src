--TEST--
PEAR_Common::downloadHttp test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
$fp = @fsockopen('pear.php.net', 80);
if (!$fp) {
    echo 'skip';
} else {
    fclose($fp);
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

echo "Simple fail:\n";
PEAR_Common::downloadHttp('http://test.poop.php.net/stuff.tgz', $ui, $temp_path);

echo "Test callback:\n";

$ui = 'My UI';

PEAR_Common::downloadHttp('http://test.pear.php.net/testdownload.tgz', $ui, $temp_path, 'myCallback');
$firstone = implode('', file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testdownload.tgz'));
$secondone = implode('', file($temp_path . DIRECTORY_SEPARATOR . 'testdownload.tgz'));
echo "Working Callback: ";
echo ($firstone == $secondone) ? "passed\n" : "failed\n";


function myCallback($message, $payload)
{
    $stuff = serialize($payload);
    echo "Callback Message: $message\n";
    echo "Callback Payload: $stuff\n";
}

echo "Callback fail:\n";
PEAR_Common::downloadHttp('http://test.poop.php.net/stuff.tgz', $ui, $temp_path, 'myCallback');

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
Test static:
Simple: passed
Simple fail:
Caught error: Connection to `test.poop.php.net:80' failed: The operation completed successfully.

Test callback:
Callback Message: setup
Callback Payload: a:1:{i:0;s:5:"My UI";}
Callback Message: message
Callback Payload: s:35:"Using HTTP proxy test.pear.php.net:";
Callback Message: saveas
Callback Payload: s:16:"testdownload.tgz";
Callback Message: start
Callback Payload: a:2:{i:0;s:16:"testdownload.tgz";i:1;s:5:"41655";}
Callback Message: bytesread
Callback Payload: i:1024;
Callback Message: bytesread
Callback Payload: i:2048;
Callback Message: bytesread
Callback Payload: i:3072;
Callback Message: bytesread
Callback Payload: i:4096;
Callback Message: bytesread
Callback Payload: i:5120;
Callback Message: bytesread
Callback Payload: i:6144;
Callback Message: bytesread
Callback Payload: i:7168;
Callback Message: bytesread
Callback Payload: i:8192;
Callback Message: bytesread
Callback Payload: i:9216;
Callback Message: bytesread
Callback Payload: i:10240;
Callback Message: bytesread
Callback Payload: i:11264;
Callback Message: bytesread
Callback Payload: i:12288;
Callback Message: bytesread
Callback Payload: i:13312;
Callback Message: bytesread
Callback Payload: i:14336;
Callback Message: bytesread
Callback Payload: i:15360;
Callback Message: bytesread
Callback Payload: i:16384;
Callback Message: bytesread
Callback Payload: i:17408;
Callback Message: bytesread
Callback Payload: i:18432;
Callback Message: bytesread
Callback Payload: i:19456;
Callback Message: bytesread
Callback Payload: i:20480;
Callback Message: bytesread
Callback Payload: i:21504;
Callback Message: bytesread
Callback Payload: i:22528;
Callback Message: bytesread
Callback Payload: i:23552;
Callback Message: bytesread
Callback Payload: i:24576;
Callback Message: bytesread
Callback Payload: i:25600;
Callback Message: bytesread
Callback Payload: i:26624;
Callback Message: bytesread
Callback Payload: i:27648;
Callback Message: bytesread
Callback Payload: i:28672;
Callback Message: bytesread
Callback Payload: i:29696;
Callback Message: bytesread
Callback Payload: i:30720;
Callback Message: bytesread
Callback Payload: i:31744;
Callback Message: bytesread
Callback Payload: i:32768;
Callback Message: bytesread
Callback Payload: i:33792;
Callback Message: bytesread
Callback Payload: i:34816;
Callback Message: bytesread
Callback Payload: i:35840;
Callback Message: bytesread
Callback Payload: i:36864;
Callback Message: bytesread
Callback Payload: i:37888;
Callback Message: bytesread
Callback Payload: i:38912;
Callback Message: bytesread
Callback Payload: i:39936;
Callback Message: bytesread
Callback Payload: i:40960;
Callback Message: bytesread
Callback Payload: i:41655;
Callback Message: done
Callback Payload: i:41655;
Working Callback: passed
Callback fail:
Callback Message: setup
Callback Payload: a:1:{i:0;s:5:"My UI";}
Callback Message: message
Callback Payload: s:35:"Using HTTP proxy test.poop.php.net:";
Callback Message: connfailed
Callback Payload: a:4:{i:0;s:17:"test.poop.php.net";i:1;i:80;i:2;i:0;i:3;s:39:"The operation completed successfully.
";}
Caught error: Connection to `test.poop.php.net:80' failed: The operation completed successfully.