--TEST--
Bug #72762: Infinite loop while parsing a file with opcache enabled
--EXTENSIONS--
opcache
--FILE--
<?php

class foo
{
    function bar()
    {
        $b = array();

        foreach ($a as $a) {
            foreach ($b as $k => $v) {
            }
            $b[$k] = $v;
        }
    }
}

?>
===DONE===
--EXPECT--
===DONE===
