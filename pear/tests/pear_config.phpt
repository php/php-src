--TEST--
PEAR_Config
--SKIPIF--
skip
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

print "\n#0 starting up\n";
dump_files();

print "\n#1 testing: constructor\n";
$config = new PEAR_Config("user.conf", "system.conf");
dump_array("files", $config->files);

print "\n#2 testing: singleton\n";
$o1 = &PEAR_Config::singleton();
$o1->blah = 'blah';
$o2 = &PEAR_Config::singleton();
var_dump($o1->blah);
@var_dump($o2->blah);

print "\n#3 testing: readConfigFile\n";
$config->readConfigFile("user2.conf", "user");
dump_config($config);
$config->readConfigFile("user.conf");
dump_config($config);

print "\n#4 testing: mergeConfigFile\n";
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

print "\n#5 testing: config file version detection\n";
$config->readConfigFile("user.conf", "user");
$config->readConfigFile("toonew.conf", "user");

print "\n#6 testing: get/set/remove\n";
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

print "\n#7 testing: getType\n";
var_dump($config->getType("__unknown__"));
var_dump($config->getType("verbose"));
var_dump($config->getType("master_server"));
var_dump($config->getType("ext_dir"));

print "\n#8 testing: getDocs\n";
print "master_server: " . $config->getDocs("master_server") . "\n";

print "\n#9 testing: getKeys\n";
$keys = $config->getKeys();
sort($keys);
print implode(" ", $keys) . "\n";

print "\n#10 testing: definedBy\n";
var_dump($config->definedBy("verbose"));
$config->set("verbose", 6, "system");
$config->set("verbose", 3, "user");
var_dump($config->definedBy("verbose"));
$config->remove("verbose", "system");
var_dump($config->definedBy("verbose"));
$config->set("verbose", 6, "system");
$config->remove("verbose", "user");
var_dump($config->definedBy("verbose"));
$config->remove("verbose", "system");
var_dump($config->definedBy("verbose"));

print "\n#11 testing: isDefined\n";
var_dump($config->isDefined("php_dir"));
var_dump($config->isDefined("verbose"));
var_dump($config->isDefined("HTTP_GET_VARS"));
var_dump($config->isDefined("query"));

print "\n#12 testing: getGroup\n";
foreach ($keys as $key) {
    print "$key: ".$config->getGroup($key)."\n";
}

print "\n#13 testing: getGroups\n";
$groups = $config->getGroups();
sort($groups);
print implode(", ", $groups) . "\n";

print "\n#14 testing: getGroupKeys\n";
foreach ($groups as $group) {
    $gk = $config->getGroupKeys($group);
    sort($gk);
    print "$group: " . implode(", ", $gk) . "\n";
}

print "\n#15 testing: getPrompt\n";
foreach ($keys as $key) {
    print "$key: ".$config->getPrompt($key)."\n";
}


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
string(4) "blah"
string(4) "blah"

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

#7 testing: getType
bool(false)
string(7) "integer"
string(6) "string"
string(9) "directory"

#8 testing: getDocs
master_server: name of the main PEAR server

#9 testing: getKeys
bin_dir cache_dir cache_ttl data_dir doc_dir ext_dir http_proxy master_server password php_dir preferred_state sig_bin sig_keydir sig_type test_dir umask username verbose

#10 testing: definedBy
string(7) "default"
string(4) "user"
string(4) "user"
string(6) "system"
string(7) "default"

#11 testing: isDefined
bool(true)
bool(true)
bool(false)
bool(false)

#12 testing: getGroup
bin_dir: File Locations
cache_dir: File Locations (Advanced)
cache_ttl: Advanced
data_dir: File Locations (Advanced)
doc_dir: File Locations
ext_dir: File Locations
http_proxy: Internet Access
master_server: Internet Access
password: Maintainers
php_dir: File Locations
preferred_state: Advanced
sig_bin: Maintainers
sig_keydir: Maintainers
sig_type: Maintainers
test_dir: File Locations (Advanced)
umask: Advanced
username: Maintainers
verbose: Advanced

#13 testing: getGroups
Advanced, File Locations, File Locations (Advanced), Internet Access, Maintainers

#14 testing: getGroupKeys
Advanced: cache_ttl, preferred_state, umask, verbose
File Locations: bin_dir, doc_dir, ext_dir, php_dir
File Locations (Advanced): cache_dir, data_dir, test_dir
Internet Access: http_proxy, master_server
Maintainers: password, sig_bin, sig_keydir, sig_type, username

#15 testing: getPrompt
bin_dir: PEAR executables directory
cache_dir: PEAR Installer cache directory
cache_ttl: Cache TimeToLive
data_dir: PEAR data directory
doc_dir: PEAR documentation directory
ext_dir: PHP extension directory
http_proxy: HTTP Proxy Server Address
master_server: PEAR server
password: PEAR password (for maintainers)
php_dir: PEAR directory
preferred_state: Preferred Package State
sig_bin: Signature Handling Program
sig_keydir: Signature Key Directory
sig_type: Package Signature Type
test_dir: PEAR test directory
umask: Unix file mask
username: PEAR username (for maintainers)
verbose: Debug Log Level
done
