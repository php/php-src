--TEST--
throw expression
--FILE--
<?php

try {
    $result = true && throw new Exception("true && throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = false && throw new Exception("false && throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = true and throw new Exception("true and throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = false and throw new Exception("false and throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = true || throw new Exception("true || throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = false || throw new Exception("false || throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = true or throw new Exception("true or throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = false or throw new Exception("false or throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = null ?? throw new Exception("null ?? throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = "foo" ?? throw new Exception('"foo" ?? throw');
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = null ?: throw new Exception("null ?: throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = "foo" ?: throw new Exception('"foo" ?: throw');
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $callable = fn() => throw new Exception("fn() => throw");
    var_dump("not yet");
    $callable();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$result = "bar";
try {
    $result = throw new Exception();
} catch (Exception $e) {}
var_dump($result);

try {
    var_dump(
        throw new Exception("exception 1"),
        throw new Exception("exception 2")
    );
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = true ? true : throw new Exception("true ? true : throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = false ? true : throw new Exception("false ? true : throw");
    var_dump($result);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    throw new Exception() + 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    throw $exception = new Exception('throw $exception = new Exception();');
} catch (Exception $e) {}
var_dump($exception->getMessage());

try {
    $exception = null;
    throw $exception ??= new Exception('throw $exception ??= new Exception();');
} catch (Exception $e) {}
var_dump($exception->getMessage());

try {
    throw null ?? new Exception('throw null ?? new Exception();');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: true && throw
bool(false)
Exception: true and throw
bool(false)
bool(true)
Exception: false || throw
bool(true)
Exception: false or throw
Exception: null ?? throw
string(3) "foo"
Exception: null ?: throw
string(3) "foo"
string(7) "not yet"
Exception: fn() => throw
string(3) "bar"
Exception: exception 1
bool(true)
Exception: false ? true : throw
TypeError: Unsupported operand types: Exception + int
string(35) "throw $exception = new Exception();"
string(37) "throw $exception ??= new Exception();"
Exception: throw null ?? new Exception();
