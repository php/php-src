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
} catch(BaseException $e) {
    var_dump($e->getMessage());
}

try {
    new $foo();
} catch(BaseException $e) {
    var_dump($e->getMessage());
}

try {
    throw $foo;
} catch(BaseException $e) {
    var_dump($e->getMessage());
}

try {
    $foo();
} catch(BaseException $e) {
    var_dump($e->getMessage());
}

try {
    $foo::b();
} catch(BaseException $e) {
    var_dump($e->getMessage());
}


try {
    $b = clone $foo;
} catch(BaseException $e) {
    var_dump($e->getMessage());
}

class b {
}

try {
    b::$foo();
} catch(BaseException $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
