--TEST--
debug_print_backtrace skip_last parameter
--FILE--
<?php
(function(){
    (function(){
        return (function() {
            debug_print_backtrace(skip_last: 0);
            echo PHP_EOL;
            debug_print_backtrace(skip_last: 1);
            echo PHP_EOL;
            debug_print_backtrace(skip_last: 2);
            echo PHP_EOL;
            debug_print_backtrace(skip_last: 3);
        })();
    })();
})();
?>
--EXPECTF--
#0 %s(5): debug_print_backtrace(0, 0, 0)
#1 %s(12): {closure}()
#2 %s(13): {closure}()
#3 %s(14): {closure}()

#0 %s(12): {closure}()
#1 %s(13): {closure}()
#2 %s(14): {closure}()

#0 %s(13): {closure}()
#1 %s(14): {closure}()

#0 %s(14): {closure}()
