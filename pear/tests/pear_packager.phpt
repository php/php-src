--TEST--
PEAR_Packager test
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
require_once "PEAR/Packager.php";

$packager = new PEAR_Packager();
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
ob_start();
$packager->package(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' . DIRECTORY_SEPARATOR . 'package.xml');
$packager->package(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' . DIRECTORY_SEPARATOR . 'package2.xml');
$stuff = str_replace(array(dirname(__FILE__) . DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR), array('', '/'),
    ob_get_contents());
ob_end_clean();
echo $stuff;

$archive1 = &new Archive_Tar(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'pkg1-1.0.tgz');
$archive1c = &new Archive_Tar(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' .
    DIRECTORY_SEPARATOR . 'pkg1-1.0.tgz');
$x1 = $archive1->listContent();
$x2 = $archive1c->listContent();
for($i=0;$i<count($x1); $i++) {
    if ($x1[$i]['filename'] == 'package.xml') {
        unset($x1[$i]['mtime']);
        unset($x1[$i]['checksum']);
    }
}
for($i=0;$i<count($x2); $i++) {
    if ($x2[$i]['filename'] == 'package.xml') {
        unset($x2[$i]['mtime']);
        unset($x2[$i]['checksum']);
    }
}
var_dump($x1, $x2);
$archive1 = &new Archive_Tar(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'pkg2-1.0.tgz');
$archive1c = &new Archive_Tar(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' .
    DIRECTORY_SEPARATOR . 'pkg2-1.0.tgz');
$x1 = $archive1->listContent();
$x2 = $archive1c->listContent();
for($i=0;$i<count($x1); $i++) {
    if ($x1[$i]['filename'] == 'package.xml') {
        unset($x1[$i]['mtime']);
        unset($x1[$i]['checksum']);
    }
}
for($i=0;$i<count($x2); $i++) {
    if ($x2[$i]['filename'] == 'package.xml') {
        unset($x2[$i]['mtime']);
        unset($x2[$i]['checksum']);
    }
}
var_dump($x1, $x2);

echo "test failure:\n";
$packager->package(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'dirtree' . DIRECTORY_SEPARATOR . 'package-fail.xml');

unlink(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'pkg1-1.0.tgz');
unlink(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'pkg2-1.0.tgz');
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
Analyzing multiplepackages/pkg1file.php
Analyzing pkg1/randomfile.php
Package pkg1-1.0.tgz done
Tag the released code with `pear cvstag package.xml'
(or set the CVS tag RELEASE_1_0 by hand)
Analyzing multiplepackages/pkg2file.php
Analyzing nestedroot/rootfile.php
Analyzing nestedroot/emptydir/nesteddir/nestedfile.php
Package pkg2-1.0.tgz done
Tag the released code with `pear cvstag package2.xml'
(or set the CVS tag RELEASE_1_0 by hand)
array(3) {
  [0]=>
  array(6) {
    ["filename"]=>
    string(11) "package.xml"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(817)
    ["typeflag"]=>
    string(0) ""
  }
  [1]=>
  array(8) {
    ["checksum"]=>
    int(5592)
    ["filename"]=>
    string(38) "pkg1-1.0/multiplepackages/pkg1file.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119017)
    ["typeflag"]=>
    string(0) ""
  }
  [2]=>
  array(8) {
    ["checksum"]=>
    int(4534)
    ["filename"]=>
    string(28) "pkg1-1.0/pkg1/randomfile.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071120110)
    ["typeflag"]=>
    string(0) ""
  }
}
array(3) {
  [0]=>
  array(6) {
    ["filename"]=>
    string(11) "package.xml"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(817)
    ["typeflag"]=>
    string(0) ""
  }
  [1]=>
  array(8) {
    ["checksum"]=>
    int(5592)
    ["filename"]=>
    string(38) "pkg1-1.0/multiplepackages/pkg1file.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119017)
    ["typeflag"]=>
    string(0) ""
  }
  [2]=>
  array(8) {
    ["checksum"]=>
    int(4534)
    ["filename"]=>
    string(28) "pkg1-1.0/pkg1/randomfile.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071120110)
    ["typeflag"]=>
    string(0) ""
  }
}
array(4) {
  [0]=>
  array(6) {
    ["filename"]=>
    string(11) "package.xml"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(921)
    ["typeflag"]=>
    string(0) ""
  }
  [1]=>
  array(8) {
    ["checksum"]=>
    int(5600)
    ["filename"]=>
    string(38) "pkg2-1.0/multiplepackages/pkg2file.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119030)
    ["typeflag"]=>
    string(0) ""
  }
  [2]=>
  array(8) {
    ["checksum"]=>
    int(5066)
    ["filename"]=>
    string(32) "pkg2-1.0/nestedroot/rootfile.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119266)
    ["typeflag"]=>
    string(0) ""
  }
  [3]=>
  array(8) {
    ["checksum"]=>
    int(7193)
    ["filename"]=>
    string(53) "pkg2-1.0/nestedroot/emptydir/nesteddir/nestedfile.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119310)
    ["typeflag"]=>
    string(0) ""
  }
}
array(4) {
  [0]=>
  array(6) {
    ["filename"]=>
    string(11) "package.xml"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(921)
    ["typeflag"]=>
    string(0) ""
  }
  [1]=>
  array(8) {
    ["checksum"]=>
    int(5600)
    ["filename"]=>
    string(38) "pkg2-1.0/multiplepackages/pkg2file.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119030)
    ["typeflag"]=>
    string(0) ""
  }
  [2]=>
  array(8) {
    ["checksum"]=>
    int(5066)
    ["filename"]=>
    string(32) "pkg2-1.0/nestedroot/rootfile.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119266)
    ["typeflag"]=>
    string(0) ""
  }
  [3]=>
  array(8) {
    ["checksum"]=>
    int(7193)
    ["filename"]=>
    string(53) "pkg2-1.0/nestedroot/emptydir/nesteddir/nestedfile.php"
    ["mode"]=>
    int(33206)
    ["uid"]=>
    int(0)
    ["gid"]=>
    int(0)
    ["size"]=>
    int(0)
    ["mtime"]=>
    int(1071119310)
    ["typeflag"]=>
    string(0) ""
  }
}
test failure:
Analyzing multiplepackages\pkg2file.php
Analyzing nestedroot\rootfile.php
Analyzing nestedroot\emptydir\nesteddir\nestedfile.php
Analyzing nestedroot\emptydir\nesteddir\doesntexist.php
Caught error: File does not exist: nestedroot\emptydir\nesteddir\doesntexist.php