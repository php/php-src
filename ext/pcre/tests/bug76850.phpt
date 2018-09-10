--TEST--
Bug #76850 Exit code mangled by set locale/preg_match
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
==DONE==
--EXPECT--
int(1)
==DONE==

