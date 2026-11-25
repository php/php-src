--TEST--
GHSA-8mjj-2r7p-4hg2: phpdbg frame display for __call trampoline
--INI--
fatal_error_backtraces=0
zend.exception_ignore_args=1
--PHPDBG--
b ZEND_CALL_TRAMPOLINE
r
f 1
f 0
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at ZEND_CALL_TRAMPOLINE]
prompt> [Breakpoint #0 in ZEND_CALL_TRAMPOLINE at %s:0, hits: 1]
 00001: <?php
 00002: class C { static function __callStatic($name, $args) {} }
prompt> [Switched to frame #1: caller() at %s:5]
>00005:     C::test(1, 2, 3, 4);
 00006: }
 00007:%w
prompt> [Switched to frame #0: C::test(1, 2, 3, 4) at %s:0]
 00001: <?php
 00002: class C { static function __callStatic($name, $args) {} }
prompt>
--FILE--
<?php
class C { static function __callStatic($name, $args) {} }

function caller() {
    C::test(1, 2, 3, 4);
}

caller();
