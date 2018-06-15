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
--EXPECT--
string(8) "Test
FOO"
string(16) "    Test
    FOO"
