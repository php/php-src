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

    if ($class == 'Dummy') {
        eval ("class Dummy implements iFoo {}");
    }


    if (interface_exists('iFoo', 0)) {
        new Dummy();
    }
    if ($class == 'iFoo') {
        eval ("interface iFoo extends iBar {}");
    }

    if ($class == 'iBar') {
        eval ("interface iBar {}");
    }
}
spl_autoload_register('my_autoloader');
new Foo();
new Dummy();
echo "okey";
?>
--EXPECT--
okey
