--TEST--
Bug #74980 (Narrowing occurred during type inference)
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

class A
{

    static function foo()
    {
        while ($undef) {
            $arr[][] = NULL;
        }

        foreach ($arr as $a) {
            bar($a + []);
        }
    }

}

echo "okey";
?>
--EXPECT--
okey
