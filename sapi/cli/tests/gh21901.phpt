--TEST--
Stale getopt() optional value in CLI
--FILE--
<?php
$php_escaped = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$cmd = $php_escaped . ' -n -d foo=bar --ini';
echo shell_exec($cmd);
?>
--EXPECTF--
Configuration File (php.ini) Path: %s
Loaded Configuration File:         %s
Scan for additional .ini files in: %s
Additional .ini files parsed:      %s
