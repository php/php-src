--TEST--
Exceptions before fatal error
--FILE--
<?php
function exception_error_handler($code, $msg) {
    throw new Exception($msg);
}

set_error_handler("exception_error_handler");

try {
    $foo->a();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new $foo();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    throw $foo;
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $foo();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $foo::b();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}


try {
    $b = clone $foo;
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class b {
}

try {
    b::$foo();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Undefined variable $foo
Exception: Undefined variable $foo
Exception: Undefined variable $foo
Exception: Undefined variable $foo
Exception: Undefined variable $foo
Exception: Undefined variable $foo
Exception: Undefined variable $foo
