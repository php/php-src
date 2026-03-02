<?php

enum Foo
{
    case Bar;
    case Baz;
    const CASES = [Foo::Bar, Foo::Baz];
}

class Qux {
    const CASES = [Foo::Bar, Foo::Baz];
}
