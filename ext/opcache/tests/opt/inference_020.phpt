--TEST--
Type inference 020;
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    for(;;) {
        $y = $y[] += 3/6 - ~$y;
    }
}
?>
DONE
--EXPECT--
DONE
