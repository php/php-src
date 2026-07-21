--TEST--
setlocale() rejects locale names with null bytes
--FILE--
<?php
class NullByteStringable {
    public function __toString(): string {
        return "C\0invalid";
    }
}

try {
    var_dump(setlocale(LC_ALL, "C\0invalid"));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, ["invalid\0locale", "C"]));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, ["invalid", "C\0invalid"]));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(setlocale(LC_ALL, [], new NullByteStringable()));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
setlocale(): Argument #2 ($locales) must not contain any null bytes
setlocale(): Argument #2 ($locales) must not contain any null bytes
setlocale(): Argument #2 ($locales) must not contain any null bytes
setlocale(): Argument #3 must not contain any null bytes
