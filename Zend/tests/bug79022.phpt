--TEST--
Bug #79022 (class_exists returns True for classes that are not ready to be used)
--FILE--
<?php
function my_autoloader($class) {
    if (class_exists('Foo', 0)) {
		new Foo();
    }
	if ($class == 'Foo') {
		eval("class Foo extends Bar{}");
	}

	if ($class == 'Bar') {
		eval("class Bar {}");
    }
}
spl_autoload_register('my_autoloader');
new Foo();
echo "okey";
?>
--EXPECT--
okey
