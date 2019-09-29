--TEST--
user defined error handler + set_error_handling(EH_THROW)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN") die("skip Not for Windows");
if (is_dir('/this/path/does/not/exist')) die("skip directory /this/path/does/not/exist already exists");
?>
--FILE--
<?php
$dir = '/this/path/does/not/exist';

set_error_handler('my_error_handler');
function my_error_handler() {$a = func_get_args(); print "in error handler\n"; }

try {
        print "before\n";
        $iter = new DirectoryIterator($dir);
        print get_class($iter) . "\n";
        print "after\n";
} catch (Exception $e) {
        print "in catch: ".$e->getMessage()."\n";
}
?>
==DONE==
<?php exit(0); ?>
--EXPECT--
before
in catch: DirectoryIterator::__construct(/this/path/does/not/exist): failed to open dir: No such file or directory
==DONE==
