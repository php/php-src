--TEST--
throw expression
--FILE--
<?php

try {
    $result = true && throw new Exception("true && throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = false && throw new Exception("false && throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = true and throw new Exception("true and throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = false and throw new Exception("false and throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = true || throw new Exception("true || throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = false || throw new Exception("false || throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = true or throw new Exception("true or throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = false or throw new Exception("false or throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = null ?? throw new Exception("null ?? throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = "foo" ?? throw new Exception('"foo" ?? throw');
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = null ?: throw new Exception("null ?: throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = "foo" ?: throw new Exception('"foo" ?: throw');
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $callable = fn() => throw new Exception("fn() => throw");
    var_dump("not yet");
    $callable();
} catch (Exception $e) {
    var_dump($e->getMessage());
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
    var_dump($e->getMessage());
}

try {
    $result = true ? true : throw new Exception("true ? true : throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $result = false ? true : throw new Exception("false ? true : throw");
    var_dump($result);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    throw new Exception() + 1;
} catch (Throwable $e) {
    var_dump($e->getMessage());
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
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(13) "true && throw"
bool(false)
string(14) "true and throw"
bool(false)
bool(true)
string(14) "false || throw"
bool(true)
string(14) "false or throw"
string(13) "null ?? throw"
string(3) "foo"
string(13) "null ?: throw"
string(3) "foo"
string(7) "not yet"
string(13) "fn() => throw"
string(3) "bar"
string(11) "exception 1"
bool(true)
string(20) "false ? true : throw"
string(42) "Unsupported operand types: Exception + int"
string(35) "throw $exception = new Exception();"
string(37) "throw $exception ??= new Exception();"
string(30) "throw null ?? new Exception();"
