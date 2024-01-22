--TEST--
Bug #68104 (Segfault while pre-evaluating a disabled function)
--CREDITS--
manuel <manuel@mausz.at>
--INI--
opcache.enable=1
opcache.enable_cli=1
disable_functions=dl
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak dl() crashes LSan');
?>
--FILE--
<?php
var_dump(is_callable("dl"));
try {
    dl("a.so");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
Call to undefined function dl()
