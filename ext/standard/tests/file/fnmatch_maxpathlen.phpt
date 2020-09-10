--TEST--
Test fnmatch() function : warning filename or pattern exceeds maxpathlen
--SKIPIF--
<?php
if (!function_exists('fnmatch')) die('skip fnmatch() function is not available');
?>
--FILE--
<?php
$longstring = str_pad('blah', PHP_MAXPATHLEN);

try {
    fnmatch('blah', $longstring);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    fnmatch($longstring, 'blah');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
fnmatch(): Argument #1 ($pattern) must have a length less than %d bytes
fnmatch(): Argument #2 ($filename) must have a length less than %d bytes
