--TEST--
Resolving method/function opline breakpoints must not leak the lookup keys
--PHPDBG--
b Foo::bar#0
b baz#0
b Foo::nope#0
b Nope::bar#0
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at Foo::bar#0]
prompt> [Breakpoint #1 added at baz#0]
prompt> [Method nope doesn't exist in class Foo]
prompt> [Pending breakpoint #3 at Nope::bar#0]
prompt>
--FILE--
<?php
class Foo {
    public function bar($x) {
        return $x + 1;
    }
}

function baz($y) {
    return $y * 2;
}
