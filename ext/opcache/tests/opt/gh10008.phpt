--TEST--
GH-10008 (Narrowing occurred during type inference of ZEND_ADD_ARRAY_ELEMENT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x20
--EXTENSIONS--
opcache
--FILE--
<?php

function test()
{
    $bool = true;
    for ($i = 0; $i < 10; $i++) {
        if ($bool !== true) {
            $string_key = "a";
            // The following line triggers narrowing during type inference of ZEND_ADD_ARRAY_ELEMENT.
            $array = ["b" => $bool, $string_key => 123];
        }

        $bool = false;
    }
}

echo "Done\n";

?>
--EXPECT--
Done
