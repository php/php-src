--TEST--
Bug #79831 (unset var in set_error_handler)
--FILE--
<?php

foreach (['+', '-'] as $t) {
    foreach ([true, false] as $o) {
        unset($a);
        set_error_handler(function () {
            unset($GLOBALS['a']);
        });
        echo ($o ?
                ($t === '+' ? $a++ : $a--) :
                ($t === '+' ? ++$a : --$a)
            ) . "OK\n";
        unset($a);
        set_error_handler(function () {
            $GLOBALS['a'] = 'I am ';
        });
        echo ($o ?
                ($t === '+' ? $a++ : $a--) :
                ($t === '+' ? ++$a : --$a)
            ) . "OK\n";
    }
}

?>
--EXPECT--
OK
OK
1OK
1OK
OK
OK
OK
OK
