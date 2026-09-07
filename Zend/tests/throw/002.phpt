--TEST--
Test throw with various expressions
--FILE--
<?php

class Foo {
    public function createNotFoundException() {
        return new Exception('Not found');
    }

    public function throwException() {
        throw $this->createNotFoundException();
    }

    public static function staticCreateNotFoundException() {
        return new Exception('Static not found');
    }

    public static function staticThrowException() {
        throw static::staticCreateNotFoundException();
    }
}

try {
    (new Foo())->throwException();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    Foo::staticThrowException();
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    throw true ? new Exception('Ternary true 1') : new Exception('Ternary true 2');
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    throw false ? new Exception('Ternary false 1') : new Exception('Ternary false 2');
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $exception1 = new Exception('Coalesce non-null 1');
    $exception2 = new Exception('Coalesce non-null 2');
    throw $exception1 ?? $exception2;
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $exception1 = null;
    $exception2 = new Exception('Coalesce null 2');
    throw $exception1 ?? $exception2;
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    throw $exception = new Exception('Assignment');
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $exception = null;
    throw $exception ??= new Exception('Coalesce assignment null');
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $exception = new Exception('Coalesce assignment non-null 1');
    throw $exception ??= new Exception('Coalesce assignment non-null 2');
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$andConditionalTest = function ($condition1, $condition2) {
    throw $condition1 && $condition2
        ? new Exception('And in conditional 1')
        : new Exception('And in conditional 2');
};

try {
    $andConditionalTest(false, false);
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $andConditionalTest(false, true);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $andConditionalTest(true, false);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $andConditionalTest(true, true);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Not found
Exception: Static not found
Exception: Ternary true 1
Exception: Ternary false 2
Exception: Coalesce non-null 1
Exception: Coalesce null 2
Exception: Assignment
Exception: Coalesce assignment null
Exception: Coalesce assignment non-null 1
Exception: And in conditional 2
Exception: And in conditional 2
Exception: And in conditional 2
Exception: And in conditional 1
