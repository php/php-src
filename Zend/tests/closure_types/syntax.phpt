--TEST--
Closure type syntax
--FILE--
<?php

function test(string $closureType) {
    try {
        eval("(function ($closureType \$c) {})(42);");
    } catch (\Error $e) {
        preg_match('(Argument #1 \\(\\$c\\) must be of type (?<type>.*), int given)', $e->getMessage(), $matches);
        echo $matches['type'] ?? $e->getMessage(), "\n";
    }
    try {
        eval("(function (): $closureType { return 42; })();");
    } catch (\Error $e) {
        preg_match('(^\\{closure\\}\\(\\): Return value must be of type (?<type>.*), int returned$)', $e->getMessage(), $matches);
        echo $matches['type'] ?? $e->getMessage(), "\n";
    }
}

test('\\Closure()');
test('\\Closure(): void');
test('\\Closure(): string');
test('\\Closure(): int|bool');
test('\\Closure(): \\Closure(): void');
test('\\Closure(): ?float');
test('\\Closure(): Foo&Bar');
test('\\Closure(): Foo|Bar');
test('\\Closure(int)');
test('\\Closure(string)');
test('\\Closure(float)');
test('\\Closure(array)');
test('\\Closure(object)');
test('\\Closure(bool)');
test('\\Closure(Foo)');
test('\\Closure(Foo, Bar)');
test('\\Closure(Foo, Bar,)');
test('\\Closure ()');
test('\\Closure(Foo&)');
test('\\Closure(Foo, Bar&)');
test('\\Closure(Foo...)');
test('\\Closure(Foo&...)');
test('\\Closure(,)');
test('\\Closure: int');
test('\\Closure(): ');
test('\\Closure(');

?>
--EXPECT--
\Closure()
\Closure()
\Closure(): void
\Closure(): void
\Closure(): string
\Closure(): string
\Closure(): int|bool
\Closure(): int|bool
\Closure(): \Closure(): void
\Closure(): \Closure(): void
\Closure(): ?float
\Closure(): ?float
\Closure(): Foo&Bar
\Closure(): Foo&Bar
\Closure(): Foo|Bar
\Closure(): Foo|Bar
\Closure(int)
\Closure(int)
\Closure(string)
\Closure(string)
\Closure(float)
\Closure(float)
\Closure(array)
\Closure(array)
\Closure(object)
\Closure(object)
\Closure(bool)
\Closure(bool)
\Closure(Foo)
\Closure(Foo)
\Closure(Foo, Bar)
\Closure(Foo, Bar)
\Closure(Foo, Bar)
\Closure(Foo, Bar)
\Closure()
\Closure()
\Closure(Foo&)
\Closure(Foo&)
\Closure(Foo, Bar&)
\Closure(Foo, Bar&)
\Closure(Foo...)
\Closure(Foo...)
\Closure(Foo&...)
\Closure(Foo&...)
syntax error, unexpected token ","
syntax error, unexpected token ","
syntax error, unexpected token ":", expecting variable
syntax error, unexpected token ":", expecting "{"
syntax error, unexpected variable "$c"
syntax error, unexpected token "{"
syntax error, unexpected variable "$c"
syntax error, unexpected token "{"
