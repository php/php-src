<?php

/* This script generates the .reg file to set up an event source for use by the php syslog() function. */

$PATH = "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\PHP-" . phpversion();

$dll = $argv[1];
$dll = addslashes($dll);

file_put_contents("win32/syslog.reg", <<<REG
REGEDIT4

[HKEY_LOCAL_MACHINE\\$PATH]
"TypesSupported"=dword:00000007
"EventMessageFile"="$dll"

REG
);

?>
