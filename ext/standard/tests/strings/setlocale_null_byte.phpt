--TEST--
setlocale() rejects locale names with null bytes
--FILE--
<?php
class NullByteStringable {
    public function __toString(): string {
        return "C\0locale";
    }
}

try {
    var_dump(setlocale(LC_ALL, "C\0locale"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, ["locale\0name", "C"]));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(@setlocale(LC_ALL, [str_repeat("x", 255), "C\0locale"]));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, "zz_ZZ.nope", ["C\0locale"]));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, "zz_ZZ.nope", new NullByteStringable()));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, [], new NullByteStringable()));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: setlocale(): Argument #2 ($locales) must not contain any null bytes
ValueError: setlocale(): Argument #2 ($locales) must not contain any null bytes
ValueError: setlocale(): Argument #2 ($locales) must not contain any null bytes
TypeError: setlocale(): Argument #3 must be of type ?string, array given
ValueError: setlocale(): Argument #3 must not contain any null bytes
ArgumentCountError: setlocale() expects exactly 2 arguments when argument #2 ($locales) is an array, 3 given
