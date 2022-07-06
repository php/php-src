--TEST--
Bug #76850 Exit code mangled by set locale/preg_match
--SKIPIF--
<?php
    $l = setlocale(LC_CTYPE, 0);
    if ("C" == $l) {
        die("skip need the current locale to be other than C");
    }
?>
--FILE--
<?php

function foo()
{
        $oldlocale = setlocale(LC_CTYPE, 0);
        setlocale(LC_CTYPE, $oldlocale);
}

foo();

var_dump(preg_match('/./', "a"));

?>
--EXPECT--
int(1)
