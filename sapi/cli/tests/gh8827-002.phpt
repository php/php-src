--TEST--
std handles can be deliberately closed 002
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

$stdin = fopen('php://stdin', 'r');
$stdout = fopen('php://stdout', 'r');
$stderr = fopen('php://stderr', 'r');

ob_start(function ($buffer) use ($stdout) {
    fwrite($stdout, $buffer);
}, 1);

print "STDIN:\n";
fclose(STDIN);
var_dump(@fopen('php://stdin', 'r'));

print "STDERR:\n";
fclose(STDERR);
var_dump(@fopen('php://stderr', 'a'));

print "STDOUT:\n";
fclose(STDOUT);
var_dump(@fopen('php://stdout', 'a'));
?>
--EXPECTF--
STDIN:

Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d
bool(false)

Deprecated: var_dump(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d
STDERR:

Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d
bool(false)

Deprecated: var_dump(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d
STDOUT:

Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d
bool(false)
