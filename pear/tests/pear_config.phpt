--TEST--
PEAR_Config
--FILE--
<?php

error_reporting(E_ALL);
include "../PEAR/Config.php";
copy("system.input", "system.conf");
copy("user.input", "user.conf");
PEAR::setErrorHandling(PEAR_ERROR_DIE, "%s\n");
dumpall();

print "creating config object\n";
$config = new PEAR_Config("user.conf", "system.conf");

// overriding system values
$config->set("master_server", "pear.localdomain");
$config->writeConfigFile();
dumpall();
var_dump($config->get("master_server"));

// going back to defaults
$config->toDefault("master_server");
$config->writeConfigFile();
dumpall();

// 

print "done\n";

unlink("user.conf");
unlink("system.conf");

// ------------------------------------------------------------------------- //

function dumpit($file)
{
	$fp = fopen($file, "r");
	print "$file:";
	$data = unserialize(fread($fp, filesize($file)));
	fclose($fp);
	if (!is_array($data)) {
		print " <empty>\n";
		return;
	}
	foreach ($data as $k => $v) {
		print " $k=\"$v\"";
	}
	print "\n";
}

function dumpall()
{
	print "dumping...\n";
	dumpit("system.conf");
	dumpit("user.conf");
}

?>
--EXPECT--
dumping...
system.conf: master_server="pear.php.net"
user.conf: <empty>
creating config object
dumping...
system.conf: master_server="pear.php.net"
user.conf: master_server="pear.localdomain"
string(16) "pear.localdomain"
dumping...
system.conf: master_server="pear.php.net"
user.conf:
done
