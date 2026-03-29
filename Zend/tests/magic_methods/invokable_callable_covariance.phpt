--TEST--
Invokable is covariant to callable in return types
--FILE--
<?php

class Base {
    public function getHandler(): callable {
        return fn() => 1;
    }
}

/* Invokable is a valid covariant return type for callable */
class Child extends Base {
    public function getHandler(): Invokable {
        return new class {
            public function __invoke(): int {
                return 2;
            }
        };
    }
}

$c = new Child();
$handler = $c->getHandler();
var_dump($handler instanceof Invokable);
var_dump($handler());

/* Concrete class with __invoke is also covariant to callable */
class Handler {
    public function __invoke(): int {
        return 3;
    }
}

class Child2 extends Base {
    public function getHandler(): Handler {
        return new Handler();
    }
}

$c2 = new Child2();
$handler2 = $c2->getHandler();
var_dump($handler2 instanceof Invokable);
var_dump($handler2());

/* Closure is covariant to callable via Invokable */
class Child3 extends Base {
    public function getHandler(): Closure {
        return fn() => 4;
    }
}

$c3 = new Child3();
$handler3 = $c3->getHandler();
var_dump($handler3 instanceof Invokable);
var_dump($handler3());

?>
--EXPECT--
bool(true)
int(2)
bool(true)
int(3)
bool(true)
int(4)
