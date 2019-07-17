<?php

/* This script sets up an event source for use by the php syslog() function. */

if (!extension_loaded("win32std")) {
	@dl("php_win32std.dll");
}

$PATH = "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\PHP-" . phpversion();

$dll = $argv[1];

if (extension_loaded("win32std")) {
	$key = @reg_create_key(HKEY_LOCAL_MACHINE, $PATH, KEY_ALL_ACCESS);

	if (!$key)
		$key = reg_open_key(HKEY_LOCAL_MACHINE, $PATH, KEY_ALL_ACCESS);

	if ($key) {
		reg_set_value($key, "TypesSupported", REG_DWORD, 7) or die("Types");
		reg_set_value($key, "EventMessageFile", REG_SZ, $dll) or die("EventMessageFile");

		syslog(LOG_NOTICE, "Registered PHP Event source");
	} else {
		echo "Could not register event source\n";
	}
}

/* let's also generate/update the bundled .reg file */

$dll = addslashes($dll);

file_put_contents("win32/syslog.reg", <<<REG
REGEDIT4

[HKEY_LOCAL_MACHINE\\$PATH]
"TypesSupported"=dword:00000007
"EventMessageFile"="$dll"

REG
);


?>
