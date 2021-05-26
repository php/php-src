--TEST--
debug_backtrace skip_last parameter
--FILE--
<?php
(function(){
    (function(){
        return (function() {
            foreach (debug_backtrace(skip_last: 0) as $frame) {
                printf("%s in %s on line %d\n", $frame["function"], $frame["file"], $frame["line"]);
            }
            echo PHP_EOL;
            foreach (debug_backtrace(skip_last: 1) as $frame) {
                printf("%s in %s on line %d\n", $frame["function"], $frame["file"], $frame["line"]);
            }
            echo PHP_EOL;
            foreach (debug_backtrace(skip_last: 2) as $frame) {
                printf("%s in %s on line %d\n", $frame["function"], $frame["file"], $frame["line"]);
            }
            echo PHP_EOL;
            foreach (debug_backtrace(skip_last: 3) as $frame) {
                printf("%s in %s on line %d\n", $frame["function"], $frame["file"], $frame["line"]);
            }
        })();
    })();
})();
?>
--EXPECTF--
debug_backtrace in %s on line 5
{closure} in %s on line 20
{closure} in %s on line 21
{closure} in %s on line 22

{closure} in %s on line 20
{closure} in %s on line 21
{closure} in %s on line 22

{closure} in %s on line 21
{closure} in %s on line 22

{closure} in %s on line 22
