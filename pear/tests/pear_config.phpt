--TEST--
PEAR_Config
--FILE--
<?php

error_reporting(E_ALL);
chdir(dirname(__FILE__));
include "../PEAR/Config.php";
copy("system.input", "system.conf");
copy("user.input", "user.conf");
PEAR::setErrorHandling(PEAR_ERROR_DIE, "%s\n");
dumpall();

print "creating config object\n";
$config = new PEAR_Config("user.conf", "system.conf");

print "overriding system values\n";
$config->set("master_server", "pear.localdomain");
$config->writeConfigFile(null, "user");
dumpall();
var_dump($config->get("master_server"));

print "going back to defaults\n";
$config->remove("master_server", "user");
$config->writeConfigFile(null, "user");
dumpall();

// 

print "done\n";

unlink("user.conf");
unlink("system.conf");

// ------------------------------------------------------------------------- //

function dumpit($file)
{
	print "$file:";
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
overriding system values
dumping...
system.conf: master_server="pear.php.net"
user.conf: master_server="pear.localdomain"
string(16) "pear.localdomain"
going back to defaults
dumping...
system.conf: master_server="pear.php.net"
user.conf: <empty>
done
