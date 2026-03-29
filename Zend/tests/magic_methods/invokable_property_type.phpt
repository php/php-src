--TEST--
Invokable can be used as a property type
--FILE--
<?php

class Handler {
    public function __invoke(int $x): int {
        return $x * 2;
    }
}

class Dispatcher {
    public Invokable $handler;
}

$d = new Dispatcher();

/* Invokable object */
$d->handler = new Handler();
var_dump(($d->handler)(5));

/* Closure is also Invokable */
$d->handler = fn(int $x): int => $x * 3;
var_dump(($d->handler)(5));

/* Non-invokable object should be rejected */
try {
    $d->handler = new stdClass();
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
int(10)
int(15)
Cannot assign stdClass to property Dispatcher::$handler of type Invokable
