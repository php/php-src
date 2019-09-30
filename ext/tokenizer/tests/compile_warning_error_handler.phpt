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
var_dump(count(token_get_all("<?php \1 echo 'Foo'; \1;")));

?>
--EXPECT--
Unexpected character in input:  '' (ASCII=1) state=0
Unexpected character in input:  '' (ASCII=1) state=0
int(10)
int(10)
