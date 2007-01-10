--TEST--
Bug #40091 (issue with spl_autoload_register() and 2 instances of the same class)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
class MyAutoloader {
	function __construct($directory_to_use) {}
	function autoload($class_name) {
		// code to autoload based on directory
	}
}

$autloader1 = new MyAutoloader('dir1');
spl_autoload_register(array($autloader1, 'autoload'));

$autloader2 = new MyAutoloader('dir2');
spl_autoload_register(array($autloader2, 'autoload'));

print_r(spl_autoload_functions());
?>
===DONE===
--EXPECT--
Array
(
    [0] => Array
        (
            [0] => MyAutoloader
            [1] => autoload
        )

    [1] => Array
        (
            [0] => MyAutoloader
            [1] => autoload
        )

)
===DONE===
