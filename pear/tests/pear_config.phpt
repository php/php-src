--TEST--
PEAR_Config
--FILE--
<?php

error_reporting(E_ALL);
chdir(dirname(__FILE__));
include "../PEAR/Config.php";
copy("system.input", "system.conf");
copy("user.input", "user.conf");
copy("user2.input", "user2.conf");
copy("merge.input", "merge.conf");
PEAR::setErrorHandling(PEAR_ERROR_PRINT, "%s\n");

print "#0 starting up\n";
dump_files();

print "#1 testing: constructor\n";
$config = new PEAR_Config("user.conf", "system.conf");
dump_array("files", $config->files);

print "#2 testing: singleton\n";
$cf2 = PEAR_Config::singleton();
dump_array("files", $cf2->files);

print "#3 testing: readConfigFile\n";
$config->readConfigFile("user2.conf", "user");
dump_config($config);
$config->readConfigFile("user.conf");
dump_config($config);

print "#4 testing: mergeConfigFile\n";
$config->readConfigFile("user2.conf");
dump_config($config, "user");
$config->mergeConfigFile("merge.conf", true);
dump_config($config, "user");
$config->readConfigFile("user2.conf");
$config->mergeConfigFile("merge.conf", false);
dump_config($config, "user");
$config->readConfigFile("user.conf");
dump_config($config, "user");
$config->mergeConfigFile("merge.conf", true, "xyzzy");

print "#5 testing: config file version detection\n";
$config->readConfigFile("user.conf", "user");
$config->readConfigFile("toonew.conf", "user");

print "#6 testing: get/set/remove\n";
var_dump($config->get("verbose"));
$config->set("verbose", 100, "system");
var_dump($config->get("verbose"));
$config->set("verbose", 2, "user");
var_dump($config->get("verbose"));
$config->set("verbose", 2, "system");
$config->set("verbose", 50, "user");
var_dump($config->get("verbose"));
$config->remove("verbose", "user");
var_dump($config->get("verbose"));
$config->remove("verbose", "system");
var_dump($config->get("verbose"));


/*
print "setting user values\n";
var_dump($config->set("master_server", "pear.localdomain"));
var_dump($config->writeConfigFile(null, "user"));
dumpall();

print "going back to defaults\n";
$config->remove("master_server", "user");
$config->writeConfigFile(null, "user");
dumpall();
*/

// 

print "done\n";

unlink("user.conf");
unlink("user2.conf");
unlink("system.conf");
unlink("merge.conf");

// ------------------------------------------------------------------------- //

function dump_file($file)
{
	print "..$file:";
        $data = PEAR_Config::_readConfigDataFrom($file);
	if (empty($data)) {
		print " <empty>\n";
		return;
	}
	foreach ($data as $k => $v) {
		print " $k=\"$v\"";
	}
	print "\n";
}

function dump_files() {
	dump_file("system.conf");
	dump_file("user.conf");
}

function dump_array($name, $arr) {
	print "$name:";
	if (empty($arr)) {
		print " <empty>";
	} else {
		foreach ($arr as $k => $v) {
			print " $k=\"$v\"";
		}
	}
	print "\n";
}

function dump_config(&$obj, $layer = null) {
	if ($layer !== null) {
		dump_array($layer, $obj->configuration[$layer]);
		return;
	}
	foreach ($obj->configuration as $layer => $data) {
		if ($layer == "default") {
			continue;
		}
		dump_array($layer, $data);
	}
}

?>
--EXPECT--
#0 starting up
..system.conf: master_server="pear.php.net"
..user.conf: <empty>
#1 testing: constructor
files: system="system.conf" user="user.conf"
#2 testing: singleton
files: system="system.conf" user="user.conf"
#3 testing: readConfigFile
user: verbose="2"
system: master_server="pear.php.net"
user: <empty>
system: master_server="pear.php.net"
#4 testing: mergeConfigFile
user: verbose="2"
user: verbose="100"
user: verbose="2"
user: <empty>
unknown config file type `xyzzy'
#5 testing: config file version detection
toonew.conf: unknown version `2.0'
#6 testing: get/set/remove
int(1)
int(100)
int(2)
int(50)
int(2)
int(1)
done
