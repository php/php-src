--TEST--
decode_html_step: Basic Decoding Tests
--FILE--
<?php

$test_cases = array(
    array("Cats &amp; Dogs", HTML_TEXT, 0, null),
    array('<abbr title="&lt;tags&gt;">', HTML_ATTRIBUTE, 13, 12),
    array('&notinyourcode', HTML_TEXT, 0, null),
    array('&notinyourcode', HTML_ATTRIBUTE, 0, null),
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
(done)