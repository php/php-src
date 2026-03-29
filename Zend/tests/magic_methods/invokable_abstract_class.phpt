--TEST--
Abstract class can implement Invokable with abstract or concrete __invoke()
--FILE--
<?php

/* Abstract class with abstract __invoke forces signature on subclasses */
abstract class TypedHandler implements Invokable {
    abstract public function __invoke(int $x): int;
}

class Doubler extends TypedHandler {
    public function __invoke(int $x): int { return $x * 2; }
}

var_dump(new Doubler() instanceof Invokable);
var_dump((new Doubler())(5));

/* Abstract class with concrete __invoke */
abstract class BaseHandler implements Invokable {
    public function __invoke(string $s): string { return strtoupper($s); }
}

class MyHandler extends BaseHandler {}

var_dump(new MyHandler() instanceof Invokable);
var_dump((new MyHandler())("hello"));

/* Abstract class implementing Invokable without __invoke at all (deferred to child) */
abstract class DeferredHandler implements Invokable {}

class ConcreteHandler extends DeferredHandler {
    public function __invoke(): string { return "concrete"; }
}

var_dump(new ConcreteHandler() instanceof Invokable);
var_dump((new ConcreteHandler())());

?>
--EXPECT--
bool(true)
int(10)
bool(true)
string(5) "HELLO"
bool(true)
string(8) "concrete"
