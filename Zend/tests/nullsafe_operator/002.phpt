--TEST--
Test nullsafe strict type check
--FILE--
<?php

try {
    false?->bar();
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

try {
    []?->bar();
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

try {
    (0)?->bar();
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

try {
    (0.0)?->bar();
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

try {
    ''?->bar();
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(39) "Call to a member function bar() on bool"
string(40) "Call to a member function bar() on array"
string(38) "Call to a member function bar() on int"
string(40) "Call to a member function bar() on float"
string(41) "Call to a member function bar() on string"
