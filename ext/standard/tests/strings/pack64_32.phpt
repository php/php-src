--TEST--
64bit pack()/unpack() tests
--SKIPIF--
<?php
if (PHP_INT_SIZE > 4) {
    die("skip 32bit test only");
}
?>
--FILE--
<?php
try {
    var_dump(pack("Q", 0));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(pack("J", 0));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(pack("P", 0));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(pack("q", 0));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(unpack("Q", ''));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unpack("J", ''));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unpack("P", ''));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unpack("q", ''));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
