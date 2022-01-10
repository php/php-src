--TEST--
SCCP 036: NAN handling
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo() {
    $y=NAN;
    for(;x;)for(;$y=1;);
}
?>
DONE
--EXPECT--
DONE
