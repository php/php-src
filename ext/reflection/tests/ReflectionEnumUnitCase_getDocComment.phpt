--TEST--
ReflectionEnumUnitCase::getDocComment()
--FILE--
<?php
// enum cases should support doc comments, like class constants.

enum Foo {
    /** Example doc comment */
    case Bar;
    case Baz;
}

var_dump((new ReflectionEnumUnitCase(Foo::class, 'Bar'))->getDocComment());
var_dump((new ReflectionEnumUnitCase(Foo::class, 'Baz'))->getDocComment());
var_dump((new ReflectionClassConstant(Foo::class, 'Bar'))->getDocComment());
var_dump((new ReflectionClassConstant(Foo::class, 'Baz'))->getDocComment());

?>
--EXPECT--
string(26) "/** Example doc comment */"
bool(false)
string(26) "/** Example doc comment */"
bool(false)
