--TEST--
strlen() with an enum argument
--SKIPIF--
<?php
//Skip if PHP version < 8.1
if (PHP_VERSION_ID < 80100) {
    die("skip enums not supported\n");
}
?>
--FILE--
<?php
//Test that passing an enum case to strlen() throws a TypeError

enum E {
    case A;
}

try {
    strlen(E::A);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
strlen(): Argument #1 ($string) must be of type string, %s given
