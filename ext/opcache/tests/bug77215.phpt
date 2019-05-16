--TEST--
Bug #77215: CFG assertion failure on multiple finalizing switch frees in one block
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function _crash($eValeur) {
    switch ($eValeur["a"]) {
        default:
            switch($eValeur["a"]) {
                default:
                    return 2;
            }
    }
}

var_dump(_crash(["a" => "b"]));
?>
--EXPECT--
int(2)
