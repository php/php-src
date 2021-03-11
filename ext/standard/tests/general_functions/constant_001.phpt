--TEST--
Test constant() function: test access to class constant
--FILE--
<?php
namespace Foo;

class Bar
{
    function dump()
    {
        var_dump(constant('static::class'));
    }
}

class Baz extends Bar
{
    function dump()
    {
        parent::dump();
        var_dump(constant('self::clAsS'));
        var_dump(constant('parent::class'));
    }
}

var_dump(constant('Not\Exists::class'));
var_dump(constant('Foo\Bar::class'));
var_dump(constant('\\Foo\Bar::class'));
(new Bar)->dump();
(new Baz)->dump();

var_dump(defined('Not\Exists::clASs'));
var_dump(defined('Foo\Bar::class'));
var_dump(defined('\\Foo\Bar::class'));
?>
--EXPECT--
string(10) "Not\Exists"
string(7) "Foo\Bar"
string(7) "Foo\Bar"
string(7) "Foo\Bar"
string(7) "Foo\Baz"
string(7) "Foo\Baz"
string(7) "Foo\Bar"
bool(true)
bool(true)
bool(true)
