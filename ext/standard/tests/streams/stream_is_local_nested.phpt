--TEST--
stream_is_local() with nested compression wrappers
--EXTENSIONS--
bz2
zlib
--FILE--
<?php

class RemoteWrapper {
    public $context;
    function stream_open($path, $mode, $options, &$opened_path) { return false; }
}
stream_wrapper_register('remote', 'RemoteWrapper', STREAM_IS_URL);

class LocalWrapper {
    public $context;
    function stream_open($path, $mode, $options, &$opened_path) { return false; }
}
stream_wrapper_register('local', 'LocalWrapper');

$targets = [
    '/etc/os-release',
    'file:///etc/os-release',
    'file://localhost/etc/os-release',
    'php://memory',
    'local://foo',
    'http://127.0.0.1/example.html',
    'ftp://127.0.0.1/example.html',
    'data:text/plain,foo',
    'remote://foo',
    'file://evil.example.com/x',
];
$prefixes = [
    '',
    'compress.zlib://',
    'compress.bzip2://',
    'compress.zlib://compress.bzip2://',
    'compress.bzip2://compress.zlib://',
    str_repeat('compress.zlib://compress.bzip2://', 20),
];

foreach ($targets as $target) {
    $results = [];
    foreach ($prefixes as $prefix) {
        $results[] = var_export(stream_is_local($prefix . $target), true);
    }
    printf("%-32s %s\n", $target, implode(' ', $results));
}

echo "\n";
var_dump(stream_is_local('unknown://foo'));
var_dump(stream_is_local('compress.zlib://unknown://foo'));

?>
--EXPECTF--
/etc/os-release                  true true true true true true
file:///etc/os-release           true true true true true true
file://localhost/etc/os-release  true true true true true true
php://memory                     true true true true true true
local://foo                      true true true true true true
http://127.0.0.1/example.html    false false false false false false
ftp://127.0.0.1/example.html     false false false false false false
data:text/plain,foo              false false false false false false
remote://foo                     false false false false false false
file://evil.example.com/x        false false false false false false


Warning: stream_is_local(): Unable to find the wrapper "unknown" - did you forget to enable it when you configured PHP? in %s on line %d
bool(true)

Warning: stream_is_local(): Unable to find the wrapper "unknown" - did you forget to enable it when you configured PHP? in %s on line %d
bool(true)
