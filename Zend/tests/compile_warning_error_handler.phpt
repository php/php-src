--TEST--
Error handler is invoked for E_COMPILE_WARNING
--FILE--
<?php

set_error_handler(function($errno, $errstr) {
    echo $errstr, "\n";
});

eval("\1 echo 'Foo\n'; \1;");

set_error_handler(function($errno, $errstr) {
    throw new Exception($errstr);
});

try {
    eval("\1 echo 'Foo'; \1;");
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Unexpected character in input:  '%s' (ASCII=1) state=0
Unexpected character in input:  '%s' (ASCII=1) state=0
Foo
Exception: Unexpected character in input:  '%s' (ASCII=1) state=0 in %s:%d
Stack trace:
#0 %s(%d): {closure}(%s)
#1 %s(%d): eval()
#2 {main}
