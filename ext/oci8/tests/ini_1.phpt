--TEST--
Test OCI8 php.ini settings
--EXTENSIONS--
oci8
--SKIPIF--
<?php
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 11)) {
    die("skip works only with Oracle 11g or greater version of Oracle client libraries");
}
?>
--INI--
oci8.privileged_connect = On
oci8.max_persistent = 111
oci8.persistent_timeout = 222
oci8.ping_interval = 333
oci8.connection_class = MyCC
oci8.events = On
oci8.statement_cache_size = 444
oci8.default_prefetch = 555
oci8.old_oci_close_semantics = On
--FILE--
<?php

echo "Test 1 - check initialization\n";
echo 'oci8.privileged_connect = ' . ini_get('oci8.privileged_connect') . "\n";
echo 'oci8.max_persistent = ' . ini_get('oci8.max_persistent') . "\n";
echo 'oci8.persistent_timeout = ' . ini_get('oci8.persistent_timeout') . "\n";
echo 'oci8.ping_interval = ' . ini_get('oci8.ping_interval') . "\n";
echo 'oci8.connection_class = ' . ini_get('oci8.connection_class') . "\n";
echo 'oci8.events = ' . ini_get('oci8.events') . "\n";
echo 'oci8.statement_cache_size = ' . ini_get('oci8.statement_cache_size') . "\n";
echo 'oci8.default_prefetch = ' . ini_get('oci8.default_prefetch') . "\n";
echo 'oci8.old_oci_close_semantics = ' . ini_get('oci8.old_oci_close_semantics') . "\n";

echo "Test 2 - check what can be changed at runtime\n";
var_dump(ini_set('oci8.privileged_connect', 'Off'));
echo 'oci8.privileged_connect = ' . ini_get('oci8.privileged_connect') . "\n";
var_dump(ini_set('oci8.max_persistent', '999'));
echo 'oci8.max_persistent = ' . ini_get('oci8.max_persistent') . "\n";
var_dump(ini_set('oci8.persistent_timeout', '888'));
echo 'oci8.persistent_timeout = ' . ini_get('oci8.persistent_timeout') . "\n";
var_dump(ini_set('oci8.ping_interval', '777'));
echo 'oci8.ping_interval = ' . ini_get('oci8.ping_interval') . "\n";
var_dump(ini_set('oci8.connection_class', 'OtherCC'));
echo 'oci8.connection_class = ' . ini_get('oci8.connection_class') . "\n";
var_dump(ini_set('oci8.events', 'Off'));
echo 'oci8.events = ' . ini_get('oci8.events') . "\n";
var_dump(ini_set('oci8.statement_cache_size', '666'));
echo 'oci8.statement_cache_size = ' . ini_get('oci8.statement_cache_size') . "\n";
var_dump(ini_set('oci8.default_prefetch', '111'));
echo 'oci8.default_prefetch = ' . ini_get('oci8.default_prefetch') . "\n";
var_dump(ini_set('oci8.old_oci_close_semantics', 'Off'));
echo 'oci8.old_oci_close_semantics = ' . ini_get('oci8.old_oci_close_semantics') . "\n";

?>
--EXPECTF--
Deprecated: Directive oci8.old_oci_close_semantics is deprecated%s
Test 1 - check initialization
oci8.privileged_connect = 1
oci8.max_persistent = 111
oci8.persistent_timeout = 222
oci8.ping_interval = 333
oci8.connection_class = MyCC
oci8.events = 1
oci8.statement_cache_size = 444
oci8.default_prefetch = 555
oci8.old_oci_close_semantics = 1
Test 2 - check what can be changed at runtime
bool(false)
oci8.privileged_connect = 1
bool(false)
oci8.max_persistent = 111
bool(false)
oci8.persistent_timeout = 222
bool(false)
oci8.ping_interval = 333
string(4) "MyCC"
oci8.connection_class = OtherCC
bool(false)
oci8.events = 1
bool(false)
oci8.statement_cache_size = 444
bool(false)
oci8.default_prefetch = 555
bool(false)
oci8.old_oci_close_semantics = 1
