--TEST--
decode_html: Basic Decoding Tests
--FILE--
<?php

$test_cases = array(
    array("&lt", HTML5_ATTRIBUTE, 0),
    array("&lt;", HTML5_ATTRIBUTE, 0),
    array("&lt,", HTML5_ATTRIBUTE, 0),
    array("&lt,", HTML5_TEXT_NODE, 0),

    array("&rightleftarrows", HTML5_ATTRIBUTE, 0),
    array("&rightleftarrows;", HTML5_ATTRIBUTE, 0),
    array("&rightleftarrows,", HTML5_ATTRIBUTE, 0),
    array("&rightleftarrows,", HTML5_TEXT_NODE, 0),

    array("&#", HTML5_TEXT_NODE, 0),
    array("&#;", HTML5_TEXT_NODE, 0),
    array("&#x;", HTML5_TEXT_NODE, 0),
    array("&#X;", HTML5_TEXT_NODE, 0),
    array("&#X", HTML5_TEXT_NODE, 0),

    array("&#11141114111;", HTML5_TEXT_NODE, 0),
    array("&#x10FFFF0000;", HTML5_TEXT_NODE, 0),

    array("&#x80;", HTML5_TEXT_NODE, 0),
    array("&#x8d;", HTML5_TEXT_NODE, 0),

    array("&#xD800;", HTML5_TEXT_NODE, 0),
    array("&#xDD70;", HTML5_TEXT_NODE, 0),

    array("&#x1f170;", HTML5_TEXT_NODE, 0),

    array("&amp&&amp&&", HTML5_TEXT_NODE, 5),
    array("&amp&&amp;&&", HTML5_TEXT_NODE, 5),
    array("&amp&&amp&&", HTML5_ATTRIBUTE, 5),
    array("&amp&&amp;&&", HTML5_ATTRIBUTE, 5),
    array("&amp&&amp=&", HTML5_TEXT_NODE, 5),
    array("&amp&&amp=&", HTML5_TEXT_NODE, 5),
    array("&amp&&amp/&", HTML5_TEXT_NODE, 5),
);

foreach ($test_cases as $test_case) {
    list($string, $context, $at) = $test_case;

    $match = decode_html($context, $string, $at, $match_length);
    if (isset($match)) {
        echo "{$match_length}:{$match}\n";
    } else {
        echo "(no match)\n";
    }
}
echo "(done)\n";

--EXPECT--
3:<
4:<
(no match)
3:<
16:⇄
17:⇄
(no match)
16:⇄
2:�
3:�
4:�
4:�
3:�
14:�
14:�
6:€
6:
8:�
8:�
9:🅰
4:&
5:&
4:&
5:&
4:&
(no match)
4:&
(done)
