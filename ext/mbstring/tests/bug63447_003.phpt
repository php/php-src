--TEST--
Bug #63447 (max_input_vars doesn't filter variables when mbstring.encoding_translation = On)
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
?>
--INI--
max_input_nesting_level=5
max_input_vars=100
mbstring.encoding_translation=1
--POST--
a=1&b[][][]=2&c[][][][][][]=7
--FILE--
<?php
print_r($_POST);
?>
--EXPECT--
Array
(
    [a] => 1
    [b] => Array
        (
            [0] => Array
                (
                    [0] => Array
                        (
                            [0] => 2
                        )

                )

        )

)
