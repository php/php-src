--TEST--
std handles can be deliberately closed 001
--SKIPIF--
<?php
if (php_sapi_name() != "cli") {
	die("skip CLI only");
}
if (PHP_OS_FAMILY == 'Windows') {
	die("skip not for Windows");
}
?>
--FILE--
<?php
print "STDIN:\n";
fclose(STDIN);
var_dump(@fopen('php://stdin', 'r'));

print "STDERR:\n";
fclose(STDERR);
var_dump(@fopen('php://stderr', 'a'));

print "STDOUT:\n";
fclose(STDOUT);
// not printed if stdout is closed
var_dump(@fopen('php://stdout', 'a'));
?>
--EXPECT--
STDIN:
bool(false)
STDERR:
bool(false)
STDOUT:
