--TEST--
GHSA-8mjj-2r7p-4hg2: phpdbg backtrace for __call trampoline with attached symbol table
--INI--
fatal_error_backtraces=0
zend.exception_ignore_args=0
--PHPDBG--
b ZEND_CALL_TRAMPOLINE
r
i v
back
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at ZEND_CALL_TRAMPOLINE]
prompt> [Breakpoint #0 in ZEND_CALL_TRAMPOLINE at %s:0, hits: 1]
 00001: <?php
 00002: class C { static function __callStatic($name, $args) {} }
prompt> [Variables in C::test() (0)]
prompt> frame #0: C::test(1, 2, 3, 4) at %s:0
frame #1: caller() at %s:5
frame #2: {main} at %s:8
prompt>
--FILE--
<?php
class C { static function __callStatic($name, $args) {} }

function caller() {
    C::test(1, 2, 3, 4);
}

caller();
