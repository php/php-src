--TEST--
decode_html: Basic Decoding Tests
--FILE--
<?php

$test_cases = array(
    array("Cats &amp; Dogs", HTML_TEXT, 0, null),
    array('<abbr title="&lt;tags&gt;">', HTML_ATTRIBUTE, 13, 12),
    array('&notinyourcode', HTML_TEXT, 0, null),
    array('&notinyourcode', HTML_ATTRIBUTE, 0, null),
    array('&#x000000000000000041', HTML_TEXT, 0, null),
    array('&#x000000000000000041;', HTML_TEXT, 0, null),
    array('&#x000000000000000041', HTML_ATTRIBUTE, 0, null),
    array('&#x000000000000000041;', HTML_ATTRIBUTE, 0, null),
    array('&#x4E&#117mbers &# can &bernou; &bernou weird', HTML_TEXT, 0, null),
    array('&#00000', HTML_TEXT, 0, null),
    array('&#00000;', HTML_TEXT, 0, null),
    array('&#xd83c;', HTML_TEXT, 0, null),
    array('The &#x91;star&#x92', HTML_ATTRIBUTE, 0, null),
);

foreach ($test_cases as $test_case) {
    list($string, $context, $at, $length) = $test_case;

    $decoded = decode_html($context, $string, $at, $length);
    $c = HTML_ATTRIBUTE === $context ? 'A' : 'T';
    $s = substr($string, $at, $length);
    echo "{$c}: '{$s}' -> '{$decoded}'\n";
}
echo "(done)\n";

--EXPECT--
T: 'Cats &amp; Dogs' -> 'Cats & Dogs'
A: '&lt;tags&gt;' -> '<tags>'
T: '&notinyourcode' -> '¬inyourcode'
A: '&notinyourcode' -> '&notinyourcode'
T: '&#x000000000000000041' -> 'A'
T: '&#x000000000000000041;' -> 'A'
A: '&#x000000000000000041' -> 'A'
A: '&#x000000000000000041;' -> 'A'
T: '&#x4E&#117mbers &# can &bernou; &bernou weird' -> 'Numbers &# can ℬ &bernou weird'
T: '&#00000' -> '�'
T: '&#00000;' -> '�'
T: '&#xd83c;' -> '�'
A: 'The &#x91;star&#x92' -> 'The ‘star’'
(done)