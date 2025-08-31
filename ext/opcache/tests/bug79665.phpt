--TEST--
Bug #79665 (ini_get() and opcache_get_configuration() may be inconsistent)
--EXTENSIONS--
opcache
--INI--
opcache.max_wasted_percentage=60
opcache.memory_consumption=7
opcache.max_accelerated_files=10
opcache.log_verbosity_level=2
--FILE--
<?php
$config = opcache_get_configuration();
var_dump(ini_get('opcache.max_wasted_percentage'));
var_dump($config['directives']['opcache.max_wasted_percentage']);
var_dump(ini_get('opcache.memory_consumption'));
var_dump($config['directives']['opcache.memory_consumption']);
var_dump(ini_get('opcache.max_accelerated_files'));
var_dump($config['directives']['opcache.max_accelerated_files']);
?>
--EXPECTF--
%s: Warning opcache.memory_consumption is set below the required 8MB.

%s: Warning opcache.max_accelerated_files is set below the required minimum (200).

%s: Warning opcache.max_wasted_percentage must be set between 1 and 50.

string(1) "5"
float(0.05)
string(3) "128"
int(134217728)
string(5) "10000"
int(10000)
