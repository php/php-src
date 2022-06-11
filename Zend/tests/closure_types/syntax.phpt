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

test('fn()');
test('fn(): void');
test('fn(): string');
test('fn(): int|bool');
test('fn(): fn(): void');
test('fn(fn())');
test('fn(): ?float');
test('fn(): Foo&Bar');
test('fn(): Foo|Bar');
test('fn(int)');
test('fn(string)');
test('fn(float)');
test('fn(array)');
test('fn(object)');
test('fn(bool)');
test('fn(Foo)');
test('fn(Foo, Bar)');
test('fn(Foo, Bar,)');
test('fn ()');
test('fn(Foo&)');
test('fn(Foo, Bar&)');
test('fn(Foo...)');
test('fn(Foo&...)');
test('fn(,)');
test('fn: int');
test('fn(): ');
test('fn(');

?>
--EXPECT--
fn()
fn()
fn(): void
fn(): void
fn(): string
fn(): string
fn(): int|bool
fn(): int|bool
fn(): fn(): void
fn(): fn(): void
fn(fn())
fn(fn())
fn(): ?float
fn(): ?float
fn(): Foo&Bar
fn(): Foo&Bar
fn(): Foo|Bar
fn(): Foo|Bar
fn(int)
fn(int)
fn(string)
fn(string)
fn(float)
fn(float)
fn(array)
fn(array)
fn(object)
fn(object)
fn(bool)
fn(bool)
fn(Foo)
fn(Foo)
fn(Foo, Bar)
fn(Foo, Bar)
fn(Foo, Bar)
fn(Foo, Bar)
fn()
fn()
syntax error, unexpected token ")"
syntax error, unexpected token ")"
syntax error, unexpected token ")"
syntax error, unexpected token ")"
fn(Foo...)
fn(Foo...)
syntax error, unexpected token "&", expecting ")"
syntax error, unexpected token "&", expecting ")"
syntax error, unexpected token ","
syntax error, unexpected token ","
syntax error, unexpected token ":"
syntax error, unexpected token ":"
syntax error, unexpected variable "$c"
syntax error, unexpected token "{"
syntax error, unexpected variable "$c"
syntax error, unexpected token "{"
