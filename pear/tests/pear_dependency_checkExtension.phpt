--TEST--
PEAR_Dependency::checkExtension() test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
if (!ini_get('enable_dl') || ini_get('safe_mode')) {
    echo 'skip';
}

$dir = ini_get('extension_dir');
if (OS_WINDOWS) {
    $suffix = '.dll';
} elseif (PHP_OS == 'HP-UX') {
    $suffix = '.sl';
} elseif (PHP_OS == 'AIX') {
    $suffix = '.a';
} elseif (PHP_OS == 'OSX') {
    $suffix = '.bundle';
} else {
    $suffix = '.so';
}

// get a list of possible extensions
$extensions = array();
if ($handle = opendir($dir)) {
    while (false !== ($file = readdir($handle))) { 
        if (strpos($file, $suffix) && substr($file, 0, 4) == 'php_') {
            $extensions[] = $file;
        }
    }
    closedir($handle); 
}

$loaded = false;
$notloaded = false;
// choose an extension for this test
foreach ($extensions as $ext) {
    $ext = substr(substr($ext, 0, strlen($ext) - strlen($suffix)), 4);
    if (!$loaded && extension_loaded($ext)) {
        $loaded = $ext;
    }
    if (!$notloaded && !extension_loaded($ext)) {
        // safe list for testing
        if (in_array($ext, array('zip', 'bz2', 'sqlite', 'dbx'))) {
            $notloaded = $ext;
        }
    }
}
if (!$notloaded || !$loaded) {
    echo 'skip';
}
?>
--FILE--
<?php

@mkdir(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp');
require_once "PEAR/Registry.php";
require_once "PEAR/Dependency.php";

$dir = ini_get('extension_dir');
if (OS_WINDOWS) {
    $suffix = '.dll';
} elseif (PHP_OS == 'HP-UX') {
    $suffix = '.sl';
} elseif (PHP_OS == 'AIX') {
    $suffix = '.a';
} elseif (PHP_OS == 'OSX') {
    $suffix = '.bundle';
} else {
    $suffix = '.so';
}

// get a list of possible extensions
$extensions = array();
if ($handle = opendir($dir)) {
    while (false !== ($file = readdir($handle))) { 
        if (strpos($file, $suffix) && substr($file, 0, 4) == 'php_') {
            $extensions[] = $file;
        }
    }
    closedir($handle); 
}

$loaded = false;
$notloaded = false;
// choose an extension for this test
foreach ($extensions as $ext) {
    $ext = substr(substr($ext, 0, strlen($ext) - strlen($suffix)), 4);
    if (!$loaded && extension_loaded($ext)) {
        $loaded = $ext;
    }
    if (!$notloaded && !extension_loaded($ext)) {
        // safe list for testing
        if (in_array($ext, array('zip', 'bz2', 'sqlite', 'dbx'))) {
            $notloaded = $ext;
        }
    }
}

$reg = new PEAR_Registry;
$reg->statedir = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp';
$dep = new PEAR_Dependency($reg);

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, null, 'has');
echo 'extension 1 ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkExtension($msg, $notloaded, null, 'not');
echo 'extension 2 ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$loadedver = phpversion($loaded);

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver + 1, 'ge');
echo 'extension 3 ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version >= " . ($loadedver + 1) .
    " is required" ? "match\n" : "$msg\n" );

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver + 1, 'ge', true);
echo 'extension 3 optional ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version >= " . ($loadedver + 1) .
    " is recommended to utilize some features" ? "match\n" : "$msg\n" );

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver, 'ne');
echo 'extension 4 ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version != " . $loadedver .
    " is required" ? "match\n" : "$msg\n" );

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver, 'gt');
echo 'extension 5 ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version > " . $loadedver .
    " is required" ? "match\n" : "$msg\n" );

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver, 'gt', true);
echo 'extension 5 optional ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version > " . $loadedver .
    " is recommended to utilize some features" ? "match\n" : "$msg\n" );

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver, 'lt');
echo 'extension 6 ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version < " . $loadedver .
    " is required" ? "match\n" : "$msg\n" );

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver, 'lt', true);
echo 'extension 6 optional ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version < " . $loadedver .
    " is recommended to utilize some features" ? "match\n" : "$msg\n" );

if ($loadedver == 0) {
    echo "extension 7 ok? no\nmessage : match\n";
    echo "extension 7 optional ok? no\nmessage : match\n";
} else {
    $msg = 'no error';
    $ret = $dep->checkExtension($msg, $loaded, $loadedver - 1, 'le');
    echo 'extension 7 ok? ';
    echo $ret ? "no\n" : "yes\n";
    echo 'message : ' . ($msg == "'$loaded' PHP extension version <= " . ($loadedver - 1).
        " is required" ? "match\n" : "$msg\n");

    $msg = 'no error';
    $ret = $dep->checkExtension($msg, $loaded, $loadedver - 1, 'le', true);
    echo 'extension 7 ok? ';
    echo $ret ? "no\n" : "yes\n";
    echo 'message : ' . ($msg == "'$loaded' PHP extension version <= " . ($loadedver - 1).
        " is recommended to utilize some features" ? "match\n" : "$msg\n");
}

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver, 'eq');
echo 'extension 8 ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkExtension($msg, $notloaded, $loadedver, 'ne');
echo 'extension 9 ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver + 1, 'eq');
echo 'extension 10 ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version == " . ($loadedver + 1).
        " is required" ? "match\n" : "$msg\n");

$msg = 'no error';
$ret = $dep->checkExtension($msg, $loaded, $loadedver + 1, 'eq', true);
echo 'extension 10 optional ok? ';
echo $ret ? "no\n" : "yes\n";
echo 'message : ' . ($msg == "'$loaded' PHP extension version == " . ($loadedver + 1).
        " is recommended to utilize some features" ? "match\n" : "$msg\n");

cleanall();
// ------------------------------------------------------------------------- //

function cleanall()
{
    $dp = opendir(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp');
    while ($ent = readdir($dp)) {
        if (substr($ent, -4) == ".reg") {
            unlink(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp' . DIRECTORY_SEPARATOR . $ent);
        }
    }
    closedir($dp);
    rmdir(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp');
}

?>
--GET--
--POST--
--EXPECT--
extension 1 ok? yes
no error
extension 2 ok? yes
no error
extension 3 ok? no
message : match
extension 3 optional ok? no
message : match
extension 4 ok? no
message : match
extension 5 ok? no
message : match
extension 5 optional ok? no
message : match
extension 6 ok? no
message : match
extension 6 optional ok? no
message : match
extension 7 ok? no
message : match
extension 7 optional ok? no
message : match
extension 8 ok? yes
no error
extension 9 ok? yes
no error
extension 10 ok? no
message : match
extension 10 optional ok? no
message : match
