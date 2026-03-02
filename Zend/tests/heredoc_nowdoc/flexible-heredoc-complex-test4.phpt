--TEST--
Flexible heredoc syntax complex test 4: interpolated variable with
the same delimiter name as the heredoc
--FILE--
<?php

{
    $FOO = "FOO";
    define("FOO", "FOO");
    $b = <<<FOO
    Test
    ${
        FOO
    }
    FOO;
    var_dump($b);
}

{
    $FOO = "FOO";
    $b = <<<FOO
        Test
        ${
        FOO
        }
    FOO;
    var_dump($b);
}

?>
--EXPECTF--
Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d

Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d
string(8) "Test
FOO"
string(16) "    Test
    FOO"
