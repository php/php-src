--TEST--
user defined error handler + set_error_handling(EH_THROW)
--SKIPIF--
<?php
	if(substr(PHP_OS, 0, 3) != "WIN") die("skip Windows only");
	if (!extension_loaded("spl") || is_dir('c:\\not\\exists\\here')) die("skip");
?>
--FILE--
<?php
$dir = 'c:\\not\\exists\\here';

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
in catch: DirectoryIterator::__construct(c:\not\exists\here,c:\not\exists\here): The system cannot find the path specified. (code: 3)
==DONE==
