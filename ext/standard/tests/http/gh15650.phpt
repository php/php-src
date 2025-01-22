--TEST--
GH-15650: http_build_query() with enum
--FILE--
<?php

enum E1: string {
    case C = 'hello world!';
}

enum E2: int {
    case C = 42;
}

enum E3 {
    case C;
}

echo http_build_query(['e1' => E1::C, 'e2' => E2::C]), "\n";

try {
    echo http_build_query(['e3' => E3::C]);
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
    echo http_build_query(E1::C);
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
e1=hello+world%21&e2=42
ValueError: Unbacked enum E3 cannot be converted to a string
TypeError: http_build_query(): Argument #1 ($data) must be of type array, E1 given
