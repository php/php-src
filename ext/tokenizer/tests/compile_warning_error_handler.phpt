--TEST--
Error handler is invoked for E_COMPILE_WARNING
--FILE--
<?php

set_error_handler(function($errno, $errstr) {
    echo $errstr, "\n";
});

var_dump(count(token_get_all("<?php \1 echo 'Foo\n'; \1;")));

set_error_handler(function($errno, $errstr) {
    throw new Exception($errstr);
});

// The exceptions get eaten
try {
    var_dump(count(token_get_all("<?php \1 echo 'Foo'; \1;")));
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Unexpected character in input:  '%s' (ASCII=1) state=0
Unexpected character in input:  '%s' (ASCII=1) state=0
int(10)
Exception: Unexpected character in input:  '%s' (ASCII=1) state=0 in %s:%d
Stack trace:
#0 [internal function]: {closure}(%s)
#1 %s(%d): token_get_all(%s)
#2 {main}
