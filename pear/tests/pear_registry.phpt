--TEST--
PEAR_Registry
--FILE--
<?php

error_reporting(E_ALL);
include dirname(__FILE__)."/../PEAR/Registry.php";
PEAR::setErrorHandling(PEAR_ERROR_DIE, "%s\n");
cleanall();

print "creating registry object\n";
$reg = new PEAR_Registry;
$reg->statedir = getcwd();
dumpall($reg);
$reg->addPackage("pkg1", array("name" => "pkg1", "version" => "1.0"));
dumpall($reg);
$reg->addPackage("pkg2", array("name" => "pkg2", "version" => "2.0"));
$reg->addPackage("pkg3", array("name" => "pkg3", "version" => "3.0"));
dumpall($reg);
$reg->updatePackage("pkg2", array("version" => "2.1"));
dumpall($reg);
var_dump($reg->deletePackage("pkg2"));
dumpall($reg);
var_dump($reg->deletePackage("pkg2"));
dumpall($reg);
$reg->updatePackage("pkg3", array("version" => "3.1b1", "status" => "beta"));
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
			print " $k=\"$v\"";
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
pkg1: version="1.0"
dump done
dumping registry...
pkg1: version="1.0"
pkg2: version="2.0"
pkg3: version="3.0"
dump done
dumping registry...
pkg1: version="1.0"
pkg2: version="2.1"
pkg3: version="3.0"
dump done
bool(true)
dumping registry...
pkg1: version="1.0"
pkg3: version="3.0"
dump done
bool(false)
dumping registry...
pkg1: version="1.0"
pkg3: version="3.0"
dump done
dumping registry...
pkg1: version="1.0"
pkg3: version="3.1b1" status="beta"
dump done
tests done
