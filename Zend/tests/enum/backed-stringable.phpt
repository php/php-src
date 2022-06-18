--TEST--
Backed string enum auto-implement Stringable
--FILE--
<?php

enum Foo: string {
    case Bar = 'Baz';
}

function print_(string $value) {
    echo $value, "\n";
}

echo (string) Foo::Bar, "\n";
echo Foo::Bar, "\n";
echo Foo::Bar . "\n";
print_(Foo::Bar);
var_dump(Foo::Bar instanceof Stringable);

?>
--EXPECT--
Baz
Baz
Baz
Baz
bool(true)
