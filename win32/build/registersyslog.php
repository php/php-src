<?php

/* This script sets up an event source for use by the php syslog() function. */

if (!extension_loaded("win32std")) {
	dl("php_win32std.dll") or die("b0rk");
}

$PATH = "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\PHP-" . phpversion();

$key = @reg_create_key(HKEY_LOCAL_MACHINE, $PATH, KEY_ALL_ACCESS);

if (!$key)
	$key = reg_open_key(HKEY_LOCAL_MACHINE, $PATH, KEY_ALL_ACCESS);

if ($key) {
	reg_set_value($key, "TypesSupported", REG_DWORD, 7) or die("Types");
	reg_set_value($key, "EventMessageFile", REG_SZ, $argv[1]) or die("EventMessageFile");

	define_syslog_variables();
	syslog(LOG_NOTICE, "Registered PHP Event source");
} else {
	echo "Could not register event source\n";
}

?>
