--TEST--
PEAR_Dependency::checkPackageUninstall() test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

require_once "PEAR/Registry.php";
require_once "PEAR/Dependency.php";

// snarfed from pear_registry.phpt
$reg = new PEAR_Registry;
@mkdir(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp');
$reg->statedir = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp';

$files1 = array(
    "pkg1-1.php" => array(
        "role" => "php",
        ),
    "pkg1-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg1",
        ),
    );
$files2 = array(
    "pkg2-1.php" => array(
        "role" => "php",
        ),
    "pkg2-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg2",
        ),
    );
$files3 = array(
    "pkg3-1.php" => array(
        "role" => "php",
        ),
    "pkg3-2.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg3",
        ),
    );

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1,
    'release_deps' => array(
        array('type' => 'pkg', 'name' => 'pkg3', 'rel' => 'not')
        )));

$dep = new PEAR_Dependency($reg);
$msg = '';
$warn = '';
$ret = $dep->checkPackageUninstall($msg, $warn, 'pkg1');
echo 'uninstall ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";
echo $warn . "\n";

cleanall();

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1,
    'release_deps' => array(
        array('type' => 'pkg', 'name' => 'pkg2', 'rel' => 'ne', 'version' => '6.0')
        )));

$dep = new PEAR_Dependency($reg);
$msg = '';
$warn = '';
$ret = $dep->checkPackageUninstall($msg, $warn, 'pkg2');
echo 'uninstall ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";
echo $warn . "\n";

cleanall();

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1,
    'release_deps' => array(
        array('type' => 'pkg', 'name' => 'pkg2', 'rel' => 'has')
        )));

$dep = new PEAR_Dependency($reg);
$msg = '';
$warn = '';
$ret = $dep->checkPackageUninstall($msg, $warn, 'pkg2');
echo 'uninstall ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";
echo $warn . "\n";

cleanall();

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1,
    'release_deps' => array(
        array('type' => 'pkg', 'name' => 'pkg2', 'rel' => 'has', 'optional' => 'no')
        )));

$dep = new PEAR_Dependency($reg);
$msg = '';
$warn = '';
$ret = $dep->checkPackageUninstall($msg, $warn, 'pkg2');
echo 'uninstall ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";
echo $warn . "\n";

cleanall();

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1,
    'release_deps' => array(
        array('type' => 'pkg', 'name' => 'pkg2', 'rel' => 'has', 'optional' => 'yes')
        )));

$dep = new PEAR_Dependency($reg);
$msg = '';
$warn = '';
$ret = $dep->checkPackageUninstall($msg, $warn, 'pkg2');
echo 'uninstall ok? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";
echo $warn . "\n";

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
uninstall ok? yes


uninstall ok? yes


uninstall ok? no
Package 'pkg1' depends on 'pkg2'


uninstall ok? no
Package 'pkg1' depends on 'pkg2'


uninstall ok? yes


Warning: Package 'pkg1' optionally depends on 'pkg2'