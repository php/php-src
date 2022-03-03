--TEST--
Register Alloction 011: Missed type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo($y) {
    for ($cnt=0;$cnt<6;$cnt++) {
        $i = $y;
        for ($i=0;$i<1;)
            for(;$i<1;)
                for(;$i<1;$i++)
                    for(;$y;);
                for($i=0;$i< 1;$i++)
                    for(;$y;);
    }
}
foo(null);
?>
DONE
--EXPECTF--
DONE