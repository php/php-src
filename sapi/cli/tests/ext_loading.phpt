--TEST--
Extension loading
--SKIPIF--
<?php
$extDir = ini_get('extension_dir');
if (!file_exists($extDir . '/opcache.so') && !file_exists($extDir . '/php_opcache.dll')) {
    die('skip Opcache shared object not found in extension_dir');
}
?>
--FILE--
<?php


function loadZendExt($extension) {
    $cmd = [
        PHP_BINARY, '-n',
        '-dextension_dir=' . ini_get('extension_dir'),
        '-dzend_extension=' . $extension,
        '-r', 'echo "Done.";'
    ];
    $proc = proc_open($cmd, [['null'], ['pipe', 'w'], ['redirect', 1]], $pipes);
    echo "Output: ", stream_get_contents($pipes[1]), "\n";
}

echo "Only extension name:\n";
loadZendExt('opcache');

echo "Name with file extension:\n";
$name = PHP_OS_FAMILY == 'Windows' ? 'php_opcache.dll' : 'opcache.so';
loadZendExt($name);

echo "Absolute path:\n";
$path = ini_get('extension_dir') . DIRECTORY_SEPARATOR . $name;
loadZendExt($path);

echo "Unknown extension name (unknown):\n";
loadZendExt('unknown_ext');

echo "Name with file extension (unknown):\n";
$name = PHP_OS_FAMILY == 'Windows' ? 'php_unknown_ext.dll' : 'unknown_ext.so';
loadZendExt($name);

echo "Absolute path (unknown):\n";
$path = ini_get('extension_dir') . DIRECTORY_SEPARATOR . $name;
loadZendExt($path);

?>
--EXPECTF--
Only extension name:
Output: Done.
Name with file extension:
Output: Done.
Absolute path:
Output: Done.
Unknown extension name (unknown):
Output: 
Warning: Failed loading Zend extension 'unknown_ext' (tried: %s) in Unknown on line 0
Done.
Name with file extension (unknown):
Output: 
Warning: Failed loading Zend extension '%Sunknown_ext%S' (tried: %s) in Unknown on line 0
Done.
Absolute path (unknown):
Output: Failed loading %s
Done.
