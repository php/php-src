--TEST--
ReflectionFunction::getConstExprId() and getConstExprClass()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Validators {
    public static function check(): string { return 'checked'; }
}

class Demo {
    #[A(static function () { return 'ok'; })]
    #[A(Validators::check(...))]
    public int $p = 0;
}

$attrs = (new ReflectionProperty(Demo::class, 'p'))->getAttributes();

// Anonymous closure declared in a constant expression: the id embeds a code
// hash, stripped here since its value is not part of the addressing contract.
$r = new ReflectionFunction($attrs[0]->getArguments()[0]);
var_dump(preg_replace('/#[0-9a-f]{8}$/', '', $r->getConstExprId()), $r->getConstExprClass());

$recreated = Closure::fromConstExpr($r->getConstExprClass(), $r->getConstExprId());
var_dump($recreated());

// First-class callable reference declared in a constant expression: the
// const-expr class is the declaring site, not the callable's scope, and
// the id is keyed by the callable's name, not by a rank.
$r = new ReflectionFunction($attrs[1]->getArguments()[0]);
var_dump($r->getConstExprId(), $r->getConstExprClass(), $r->getClosureScopeClass()->name);

$recreated = Closure::fromConstExpr($r->getConstExprClass(), $r->getConstExprId());
var_dump($recreated());

// Null for anything that is not declared in a constant expression
var_dump((new ReflectionFunction(function () {}))->getConstExprId());
var_dump((new ReflectionFunction(strlen(...)))->getConstExprId());
var_dump((new ReflectionFunction(Validators::check(...)))->getConstExprId());
var_dump((new ReflectionFunction('strlen'))->getConstExprId());
var_dump((new ReflectionFunction('strlen'))->getConstExprClass());

?>
--EXPECT--
string(4) "$p@0"
string(4) "Demo"
string(2) "ok"
string(20) "$p@Validators::check"
string(4) "Demo"
string(10) "Validators"
string(7) "checked"
NULL
NULL
NULL
NULL
NULL
