--TEST--
bug_debug_backtrace.phpt with replaced zend_execute_ex
--EXTENSIONS--
zend_test
--INI--
zend_test.replace_zend_execute_ex=1
opcache.jit=disable
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
--EXPECTF--
#0 %s(%d): boo()
#1 %s(%d): bar()
#2 %s(%d) : eval()'d code(1): foo()
#3 %s(%d): eval()
Done
