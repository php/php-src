--TEST--
Bug - crash in debug_backtrace when trace starts in eval
--FILE--
<?php
function foo() {
    bar();
}

function bar() {
    boo();
}

function boo(){
    debug_print_backtrace();
}

eval("foo();");

echo "Done\n";
?>
===DONE===
--EXPECTF--
#0  boo() called at [%s:%d]
#1  bar() called at [%s:%d]
#2  foo() called at [%s(%d) : eval()'d code:1]
#3  eval() called at [%s:%d]
Done
===DONE===
