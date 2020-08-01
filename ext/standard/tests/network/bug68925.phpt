--TEST--
Bug #68925 (CVE-2015-0235 – GHOST: glibc gethostbyname buffer overflow)
--FILE--
<?php
try {
    var_dump(gethostbyname(str_repeat("0", 2501)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gethostbynamel(str_repeat("0", 2501)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
gethostbyname(): Argument #1 ($hostname) cannot be longer than 255 characters
gethostbynamel(): Argument #1 ($hostname) cannot be longer than 255 characters
