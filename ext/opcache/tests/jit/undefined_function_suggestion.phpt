--TEST--
JIT: undefined function errors include a name suggestion
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1205
opcache.jit_buffer_size=64M
--FILE--
<?php
namespace App;

function helper() {}

function test() {
    try { \strlenn("x"); } catch (\Error $e) { echo $e->getMessage(), "\n"; }
    try { strlenn("x"); } catch (\Error $e) { echo $e->getMessage(), "\n"; }
    try { helpr(); } catch (\Error $e) { echo $e->getMessage(), "\n"; }
    try { nonexistentfunc(); } catch (\Error $e) { echo $e->getMessage(), "\n"; }
}

test();
?>
--EXPECT--
Call to undefined function strlenn() (did you mean strlen?)
Call to undefined function App\strlenn() (did you mean strlen?)
Call to undefined function App\helpr() (did you mean App\helper?)
Call to undefined function App\nonexistentfunc()
