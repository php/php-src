--TEST--
ZE2 A static abstract methods
--FILE--
<?php

interface showable
{
    static function show();
}

class pass implements showable
{
    static function show() {
        echo "Call to function show()\n";
    }
}

pass::show();

eval('
class fail
{
    abstract static function func();
}
');

fail::show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Class fail declares abstract method func() and must therefore be declared abstract in %sabstract_static.php(%d) : eval()'d code on line %d
