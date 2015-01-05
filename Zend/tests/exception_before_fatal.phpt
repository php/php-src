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
} catch(Exception $e) {
    var_dump($e->getMessage());
}

try {
    new $foo();
} catch(Exception $e) {
    var_dump($e->getMessage());
}

try {
    throw $foo;
} catch(Exception $e) {
    var_dump($e->getMessage());
}

try {
    $foo();
} catch(Exception $e) {
    var_dump($e->getMessage());
}

try {
    $foo::b();
} catch(Exception $e) {
    var_dump($e->getMessage());
}


try {
    $b = clone $foo;
} catch(Exception $e) {
    var_dump($e->getMessage());
}

class b {
}

try {
    b::$foo();
} catch(Exception $e) {
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
