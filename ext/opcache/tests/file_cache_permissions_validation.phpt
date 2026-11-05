--TEST--
opcache.file_cache_permissions only accepts octal modes from 0600 to 0770 without permissions for others
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--INI--
opcache.file_cache_permissions=0640
--FILE--
<?php
foreach (['0400', '01770', '0644', '-1', '0o660', '660'] as $permissions) {
    $cmd = [
        PHP_BINARY, '-n',
        '-d', 'opcache.log_verbosity_level=2',
        '-d', 'opcache.file_cache_permissions=' . $permissions,
        '-r', 'var_dump(decoct(opcache_get_configuration()["directives"]["opcache.file_cache_permissions"]));',
    ];
    $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    echo stream_get_contents($pipes[1]);
    proc_close($proc);
}

var_dump(decoct(opcache_get_configuration()["directives"]["opcache.file_cache_permissions"]));
?>
--EXPECTF--
%sWarning opcache.file_cache_permissions must be an octal mode from 0600 to 0770 without permissions for others, "0400" given.

string(3) "600"
%sWarning opcache.file_cache_permissions must be an octal mode from 0600 to 0770 without permissions for others, "01770" given.

string(3) "600"
%sWarning opcache.file_cache_permissions must be an octal mode from 0600 to 0770 without permissions for others, "0644" given.

string(3) "600"
%sWarning opcache.file_cache_permissions must be an octal mode from 0600 to 0770 without permissions for others, "-1" given.

string(3) "600"
%sWarning opcache.file_cache_permissions must be an octal mode from 0600 to 0770 without permissions for others, "0o660" given.

string(3) "600"
string(3) "660"
string(3) "640"
