--TEST--
Bug #41638 (pcre 7.0 regression)
--FILE--
<?php
$str = "repeater id='loopt' dataSrc=subject columns=2";

preg_match_all("/(['\"])((.*(\\\\\\1)*)*)\\1/sU",$str,$str_instead);
print_r($str_instead);

// these two are from Magnus Holmgren (extracted from a pcre-dev mailing list post)
preg_match_all("/(['\"])((?:\\\\\\1|.)*)\\1/sU", $str, $str_instead);
print_r($str_instead);

preg_match_all("/(['\"])(.*)(?<!\\\\)\\1/sU", $str, $str_instead);
print_r($str_instead);

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [0] => 'loopt'
        )

    [1] => Array
        (
            [0] => '
        )

    [2] => Array
        (
            [0] => loopt
        )

    [3] => Array
        (
            [0] => t
        )

    [4] => Array
        (
            [0] => 
        )

)
Array
(
    [0] => Array
        (
            [0] => 'loopt'
        )

    [1] => Array
        (
            [0] => '
        )

    [2] => Array
        (
            [0] => loopt
        )

)
Array
(
    [0] => Array
        (
            [0] => 'loopt'
        )

    [1] => Array
        (
            [0] => '
        )

    [2] => Array
        (
            [0] => loopt
        )

)
