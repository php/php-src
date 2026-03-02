--TEST--
Bug #75681: Warning: Narrowing occurred during type inference (specific case)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

function foobar()
{
    do {
        foreach ($a as $i => $_) {
            $a[$i][0] += 1;
        }

        $a[] = array(0, 0);
    } while ($x !== false);
}

?>
===DONE===
--EXPECT--
===DONE===
