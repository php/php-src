--TEST--
GH-10497: Writing to a property of a non-object constant reports a runtime error
--FILE--
<?php

try {
    TRUE->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    NULL->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    PHP_INT_MAX->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class C {
    const INT = 5;
    const STR = 'str';
    const ARR = [1, 2];

    public static function fromSelf(): void {
        self::INT->prop = 1;
    }
}

try {
    C::INT->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    C::STR->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    C::ARR->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    C::fromSelf();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Other write contexts: compound assignment, unset() and by-reference arguments.
try {
    C::INT->prop++;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    C::INT->prop .= 'x';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

function byRef(&$v) {}

try {
    byRef(C::INT->prop);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// As for plain variables, unsetting a property of a non-object is a silent no-op.
unset(C::INT->prop);
echo "unset() did not error\n";

var_dump(TRUE, C::INT, C::STR);
var_dump(isset(C::INT->prop));

try {
    __COMPILER_HALT_OFFSET__->prop = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

__halt_compiler();

?>
--EXPECT--
Error: Attempt to assign property "prop" on true
Error: Attempt to assign property "prop" on null
Error: Attempt to assign property "prop" on int
Error: Attempt to assign property "prop" on int
Error: Attempt to assign property "prop" on string
Error: Attempt to assign property "prop" on array
Error: Attempt to assign property "prop" on int
Error: Attempt to increment/decrement property "prop" on int
Error: Attempt to assign property "prop" on int
Error: Attempt to modify property "prop" on int
unset() did not error
bool(true)
int(5)
string(3) "str"
bool(false)
Error: Attempt to assign property "prop" on int
