--TEST--
PEAR_Dependency::checkPackage() test
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

mkdir(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'checkPackagetmp');
// snarfed from pear_registry.phpt
$reg = new PEAR_Registry;
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
$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1));

$dep = new PEAR_Dependency($reg);
$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1');
echo 'has works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'eq');
echo 'eq 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'le');
echo 'le 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.1', 'lt');
echo 'lt 1.1 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.1', 'ne');
echo 'ne 1.1 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'ge');
echo 'ge 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '0.9', 'gt');
echo 'ge 0.9 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg2', null, 'not');
echo 'not pkg2 works? ';
echo $ret ? "no\n" : "yes\n";
echo $msg . "\n";

// error conditions

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg2', null, 'has');
echo 'has pkg2 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_MISSING? ';
echo ($ret == PEAR_DEPENDENCY_MISSING) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg2', null, 'has', true);
echo 'has optional pkg2 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_MISSING_OPTIONAL? ';
echo ($ret == PEAR_DEPENDENCY_MISSING_OPTIONAL) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '0.9', 'le');
echo 'le 0.9 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_CONFLICT? ';
echo ($ret == PEAR_DEPENDENCY_CONFLICT) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '0.9', 'le', true);
echo 'optional le 0.9 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_CONFLICT_OPTIONAL? ';
echo ($ret == PEAR_DEPENDENCY_CONFLICT_OPTIONAL) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'ne');
echo 'ne 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_CONFLICT? ';
echo ($ret == PEAR_DEPENDENCY_CONFLICT) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'ne', true);
echo 'optional ne 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_CONFLICT_OPTIONAL? ';
echo ($ret == PEAR_DEPENDENCY_CONFLICT_OPTIONAL) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.1', 'ge');
echo 'ge 1.1 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_UPGRADE_MINOR? ';
echo ($ret == PEAR_DEPENDENCY_UPGRADE_MINOR) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.1', 'ge', true);
echo 'optional ge 1.1 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_UPGRADE_MINOR_OPTIONAL? ';
echo ($ret == PEAR_DEPENDENCY_UPGRADE_MINOR_OPTIONAL) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '2.0', 'ge');
echo 'ge 2.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_UPGRADE_MAJOR? ';
echo ($ret == PEAR_DEPENDENCY_UPGRADE_MAJOR) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '2.0', 'ge', true);
echo 'optional ge 2.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_UPGRADE_MAJOR_OPTIONAL? ';
echo ($ret == PEAR_DEPENDENCY_UPGRADE_MAJOR_OPTIONAL) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'gt');
echo 'gt 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_UPGRADE_MINOR? ';
echo ($ret == PEAR_DEPENDENCY_UPGRADE_MINOR) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', '1.0', 'gt', true);
echo 'optional gt 1.0 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_UPGRADE_MINOR_OPTIONAL? ';
echo ($ret == PEAR_DEPENDENCY_UPGRADE_MINOR_OPTIONAL) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', null, 'not');
echo 'not pkg1 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_CONFLICT? ';
echo ($ret == PEAR_DEPENDENCY_CONFLICT) ? "yes\n" : "no\n";
echo $msg . "\n";

$msg = 'no error';
$ret = $dep->checkPackage($msg, 'pkg1', null, 'foobar');
echo 'foobar pkg1 works? ';
echo $ret ? "no\n" : "yes\n";
echo '$ret is PEAR_DEPENDENCY_BAD_DEPENDENCY? ';
echo ($ret == PEAR_DEPENDENCY_BAD_DEPENDENCY) ? "yes\n" : "no\n";
echo $msg . "\n";
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
has works? yes
no error
eq 1.0 works? yes
no error
le 1.0 works? yes
no error
lt 1.1 works? yes
no error
ne 1.1 works? yes
no error
ge 1.0 works? yes
no error
ge 0.9 works? yes
no error
not pkg2 works? yes
no error
has pkg2 works? no
$ret is PEAR_DEPENDENCY_MISSING? yes
requires package `pkg2'
has optional pkg2 works? no
$ret is PEAR_DEPENDENCY_MISSING_OPTIONAL? yes
package `pkg2' is recommended to utilize some features.
le 0.9 works? no
$ret is PEAR_DEPENDENCY_CONFLICT? yes
requires package `pkg1' <= 0.9
optional le 0.9 works? no
$ret is PEAR_DEPENDENCY_CONFLICT_OPTIONAL? yes
package `pkg1' version <= 0.9 is recommended to utilize some features.  Installed version is 1.0
ne 1.0 works? no
$ret is PEAR_DEPENDENCY_CONFLICT? yes
requires package `pkg1' != 1.0
optional ne 1.0 works? no
$ret is PEAR_DEPENDENCY_CONFLICT_OPTIONAL? yes
package `pkg1' version != 1.0 is recommended to utilize some features.  Installed version is 1.0
ge 1.1 works? no
$ret is PEAR_DEPENDENCY_UPGRADE_MINOR? yes
requires package `pkg1' >= 1.1
optional ge 1.1 works? no
$ret is PEAR_DEPENDENCY_UPGRADE_MINOR_OPTIONAL? yes
package `pkg1' version >= 1.1 is recommended to utilize some features.  Installed version is 1.0
ge 2.0 works? no
$ret is PEAR_DEPENDENCY_UPGRADE_MAJOR? yes
requires package `pkg1' >= 2.0
optional ge 2.0 works? no
$ret is PEAR_DEPENDENCY_UPGRADE_MAJOR_OPTIONAL? yes
package `pkg1' version >= 2.0 is recommended to utilize some features.  Installed version is 1.0
gt 1.0 works? no
$ret is PEAR_DEPENDENCY_UPGRADE_MINOR? yes
requires package `pkg1' > 1.0
optional gt 1.0 works? no
$ret is PEAR_DEPENDENCY_UPGRADE_MINOR_OPTIONAL? yes
package `pkg1' version > 1.0 is recommended to utilize some features.  Installed version is 1.0
not pkg1 works? no
$ret is PEAR_DEPENDENCY_CONFLICT? yes
conflicts with package `pkg1'
foobar pkg1 works? no
$ret is PEAR_DEPENDENCY_BAD_DEPENDENCY? yes
relation 'foobar' with requirement '' is not supported (name=pkg1)
