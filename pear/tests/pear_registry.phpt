--TEST--
PEAR_Registry
--SKIPIF--
skip
--FILE--
<?php

error_reporting(E_ALL);
include dirname(__FILE__)."/../PEAR/Registry.php";
PEAR::setErrorHandling(PEAR_ERROR_DIE, "%s\n");
cleanall();

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
$files3_new = array(
    "pkg3-3.php" => array(
        "role" => "php",
        "baseinstalldir" => "pkg3",
        ),
    "pkg3-4.php" => array(
        "role" => "php",
        ),
    );

print "creating registry object\n";
$reg = new PEAR_Registry;
$reg->statedir = getcwd();
dumpall($reg);

$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0", "filelist" => $files1));
dumpall($reg);

$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0", "filelist" => $files2));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0", "filelist" => $files3));
dumpall($reg);

$reg->updatePackage("pkg2", array("version" => "2.1"));
dumpall($reg);

var_dump($reg->deletePackage("pkg2"));
dumpall($reg);

var_dump($reg->deletePackage("pkg2"));
dumpall($reg);

$reg->updatePackage("pkg3", array("version" => "3.1b1", "status" => "beta"));
dumpall($reg);

print_r($reg->checkFilemap(array_merge($files3, $files2)));

$reg->updatePackage("pkg3", array("filelist" => $files3_new));
dumpall($reg);

print "tests done\n";

cleanall();

// ------------------------------------------------------------------------- //

function cleanall()
{
    $dp = opendir(".");
    while ($ent = readdir($dp)) {
        if (substr($ent, -4) == ".reg") {
            unlink($ent);
        }
    }
}

function dumpall(&$reg)
{
    print "dumping registry...\n";
    $info = $reg->packageInfo();
    foreach ($info as $pkg) {
        print $pkg["name"] . ":";
        unset($pkg["name"]);
        foreach ($pkg as $k => $v) {
            if ($k == '_lastmodified') continue;
            if (is_array($v) && $k == 'filelist') {
                print " $k=array(";
                $i = 0;
                foreach ($v as $k2 => $v2) {
                    if ($i++ > 0) print ",";
                    print "{$k2}[";
                    $j = 0;
                    foreach ($v2 as $k3 => $v3) {
                        if ($j++ > 0) print ",";
                        print "$k3=$v3";
                    }
                    print "]";
                }
                print ")";
            } else {
                print " $k=\"$v\"";
            }
        }
        print "\n";
    }
    print "dump done\n";
}

?>
--EXPECT--
creating registry object
dumping registry...
dump done
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
dump done
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg2: version="2.0" filelist=array(pkg2-1.php[role=php],pkg2-2.php[role=php,baseinstalldir=pkg2])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg2: version="2.1" filelist=array(pkg2-1.php[role=php],pkg2-2.php[role=php,baseinstalldir=pkg2])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
bool(true)
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
bool(false)
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.0" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3])
dump done
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-1.php[role=php],pkg3-2.php[role=php,baseinstalldir=pkg3]) status="beta"
dump done
Array
(
    [pkg3-1.php] => pkg3
    [pkg3/pkg3-2.php] => pkg3
)
dumping registry...
pkg1: version="1.0" filelist=array(pkg1-1.php[role=php],pkg1-2.php[role=php,baseinstalldir=pkg1])
pkg3: version="3.1b1" filelist=array(pkg3-3.php[role=php,baseinstalldir=pkg3],pkg3-4.php[role=php]) status="beta"
dump done
tests done
