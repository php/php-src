--TEST--
Test basic nullsafe method calls
--FILE--
<?php

class Foo {
    function null() {
        var_dump('Foo::null()');
        return null;
    }

    function self() {
        var_dump('Foo::self()');
        return $this;
    }
}

var_dump(null?->bar());
var_dump(null?->bar(var_dump('Not executed')));
var_dump(null?->bar()->baz());
var_dump(null?->bar()->baz(var_dump('Not executed')));
var_dump(null?->bar()->baz);
var_dump(null?->bar()::$baz);
var_dump(null?->bar()::baz());

$foo = new Foo();
var_dump($foo->null()?->bar());
var_dump($foo->null()?->bar(var_dump('Not executed')));
var_dump($foo->null()?->bar()->baz());
var_dump($foo->null()?->bar()->baz(var_dump('Not executed')));
var_dump($foo->null()?->bar()->baz);
var_dump($foo->null()?->bar()::$baz);
var_dump($foo->null()?->bar()::baz());

$foo = new Foo();
var_dump($foo?->self(var_dump('Executed'))->null()?->bar());
var_dump($foo?->self(var_dump('Executed'))->null()?->bar(var_dump('Not executed')));
var_dump($foo?->self(var_dump('Executed'))->null()?->bar()->baz());
var_dump($foo?->self(var_dump('Executed'))->null()?->bar()->baz(var_dump('Not executed')));
var_dump($foo?->self(var_dump('Executed'))->null()?->bar()->baz);
var_dump($foo?->self(var_dump('Executed'))->null()?->bar()::$baz);
var_dump($foo?->self(var_dump('Executed'))->null()?->bar()::baz());

var_dump($foo->self(null?->bar())->null());
try {
    var_dump($foo?->self()[null?->bar()]);
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
NULL
NULL
NULL
NULL
NULL
NULL
NULL
string(11) "Foo::null()"
NULL
string(11) "Foo::null()"
NULL
string(11) "Foo::null()"
NULL
string(11) "Foo::null()"
NULL
string(11) "Foo::null()"
NULL
string(11) "Foo::null()"
NULL
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(8) "Executed"
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(11) "Foo::self()"
string(11) "Foo::null()"
NULL
string(11) "Foo::self()"
string(38) "Cannot use object of type Foo as array"
