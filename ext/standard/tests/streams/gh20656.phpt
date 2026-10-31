--TEST--
GH-20656 (HashTable use-after-destroy in function table via stream wrapper magic __call() with nested redeclaration)
--FILE--
<?php
class Loader {
    function stream_open() {
        return true;
    }
    function __call($m, $a) {
        $map[$obj] = putenv("LA=Gx$g_lang");
        function __call($m, $a) {}
    }
}
stream_wrapper_register('abc', 'Loader');
require 'abc://';
?>
--EXPECT--
