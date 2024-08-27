--TEST--
decode_html: Basic Decoding Tests
--FILE--
<?php

$test_cases = array(
    array("Cats &amp; Dogs", HtmlContext::BodyText, 0, null),
    array('<abbr title="&lt;tags&gt;">', HtmlContext::Attribute, 13, 12),
    array('&notinyourcode', HtmlContext::BodyText, 0, null),
    array('&notinyourcode', HtmlContext::Attribute, 0, null),
    array('&#x000000000000000041', HtmlContext::BodyText, 0, null),
    array('&#x000000000000000041;', HtmlContext::BodyText, 0, null),
    array('&#x000000000000000041', HtmlContext::Attribute, 0, null),
    array('&#x000000000000000041;', HtmlContext::Attribute, 0, null),
    array('&#x4E&#117mbers &# can &bernou; &bernou weird', HtmlContext::BodyText, 0, null),
    array('&#00000', HtmlContext::BodyText, 0, null),
    array('&#00000;', HtmlContext::BodyText, 0, null),
    array('&#xd83c;', HtmlContext::BodyText, 0, null),
    array("\r\n is \n and remaining \r are \n", HtmlContext::BodyText, 0, null),
    array("One\x00Two", HtmlContext::Attribute, 0, null),
    array("One\x00Two", HtmlContext::BodyText, 0, null),
    array("One\x00Two", HtmlContext::ForeignText, 0, null),
    array("One\x00Two", HtmlContext::Script, 0, null),
    array("One\x00Two", HtmlContext::Style, 0, null),
    array('The &#x91;star&#x92', HtmlContext::Attribute, 0, null),
    array('alert("&amp;")', HtmlContext::Script, 0, null),
    array('a::before { content: "&lt;a&gt;"; }', HtmlContext::Style, 0, null),
);

foreach ($test_cases as $test_case) {
    list($string, $context, $at, $length) = $test_case;

    $decoded = decode_html($context, $string, $at, $length);
    $c = (function ($ctx) {
        switch ($ctx) {
            case HtmlContext::Attribute: return 'A';
            case HtmlContext::BodyText: return 'T';
            case HtmlContext::ForeignText: return 'F';
            case HtmlContext::Script: return 'Sc';
            case HtmlContext::Style: return 'St';
        }
    })($context);
    $s = substr($string, $at, $length);
    $s = str_replace(array("\x00", "\r", "\n"), array("␀", "␍", "␤"), $s);
    $decoded = str_replace(array("\x00", "\r", "\n"), array("␀", "␍", "␤"), $decoded);
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
T: '␍␤ is ␤ and remaining ␍ are ␤' -> '␤ is ␤ and remaining ␤ are ␤'
A: 'One␀Two' -> 'One�Two'
T: 'One␀Two' -> 'OneTwo'
F: 'One␀Two' -> 'One�Two'
Sc: 'One␀Two' -> 'One�Two'
St: 'One␀Two' -> 'One�Two'
A: 'The &#x91;star&#x92' -> 'The ‘star’'
Sc: 'alert("&amp;")' -> 'alert("&amp;")'
St: 'a::before { content: "&lt;a&gt;"; }' -> 'a::before { content: "&lt;a&gt;"; }'
(done)
