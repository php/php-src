--TEST--
GH-10008: Narrowing occurred during type inference of ZEND_ADD_ARRAY_ELEMENT
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php
function test()
{
    $bool_or_int = true;
    while (a()) {
        if ($bool_or_int !== true) {
            // The following line triggers narrowing during type inference of ZEND_ADD_ARRAY_ELEMENT.
            $string_key = "a";
            $array = ["bool_or_int" => $bool_or_int, $string_key => 123];
        }

        $bool_or_int = 0;
    }
}
?>
DONE
--EXPECT--
DONE
