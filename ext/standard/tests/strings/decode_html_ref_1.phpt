--TEST--
decode_html_ref: Basic Decoding Tests
--FILE--
<?php

$test_cases = array(
    array("&AElig;", HtmlContext::BodyText, 0),

    array("&lt", HtmlContext::Attribute, 0),
    array("&lt;", HtmlContext::Attribute, 0),
    array("&lt,", HtmlContext::Attribute, 0),
    array("&lt,", HtmlContext::BodyText, 0),

    array("&rightleftarrows", HtmlContext::Attribute, 0),
    array("&rightleftarrows;", HtmlContext::Attribute, 0),
    array("&rightleftarrows,", HtmlContext::Attribute, 0),
    array("&rightleftarrows,", HtmlContext::BodyText, 0),

    array("&#", HtmlContext::BodyText, 0),
    array("&#;", HtmlContext::BodyText, 0),
    array("&#x;", HtmlContext::BodyText, 0),
    array("&#X;", HtmlContext::BodyText, 0),
    array("&#X", HtmlContext::BodyText, 0),

    array("&#11141114111;", HtmlContext::BodyText, 0),
    array("&#x10FFFF0000;", HtmlContext::BodyText, 0),

    array("&#x80;", HtmlContext::BodyText, 0),
    array("&#x8d;", HtmlContext::BodyText, 0),

    array("&#xD800;", HtmlContext::BodyText, 0),
    array("&#xDD70;", HtmlContext::BodyText, 0),

    array("&#x1f170;", HtmlContext::BodyText, 0),

    array("&amp;=", HtmlContext::Attribute, 0),
    array("&amp=", HtmlContext::Attribute, 0),
    array("&amp=", HtmlContext::BodyText, 0),

    // &cent is allowed in unambiguous contexts without the ; but
    // it's also a substring of &centerdot; which requires the ;.
    array("&centerdot", HtmlContext::Attribute, 0),
    array("&centerdot", HtmlContext::BodyText, 0),

    array("&amp&&amp&&", HtmlContext::BodyText, 5),
    array("&amp&&amp;&&", HtmlContext::BodyText, 5),
    array("&amp&&amp&&", HtmlContext::Attribute, 5),
    array("&amp&&amp;&&", HtmlContext::Attribute, 5),
    array("&amp&&amp=&", HtmlContext::BodyText, 5),
    array("&amp&&amp=&", HtmlContext::BodyText, 5),
    array("&amp&&amp/&", HtmlContext::BodyText, 5),
);

foreach ($test_cases as $test_case) {
    list($string, $context, $at) = $test_case;

    $match = decode_html_ref($context, $string, $at, $match_length);
    $c = HtmlContext::Attribute === $context ? 'A' : 'T';
    if (isset($match)) {
        echo "{$c}(@{$at} {$string}) {$match_length}:{$match}\n";
    } else {
        echo "{$c}(@{$at} {$string}) (no match)\n";
    }
}
echo "(done)\n";

--EXPECT--
T(@0 &AElig;) 7:Æ
A(@0 &lt) 3:<
A(@0 &lt;) 4:<
A(@0 &lt,) 3:<
T(@0 &lt,) 3:<
A(@0 &rightleftarrows) (no match)
A(@0 &rightleftarrows;) 17:⇄
A(@0 &rightleftarrows,) (no match)
T(@0 &rightleftarrows,) (no match)
T(@0 &#) (no match)
T(@0 &#;) (no match)
T(@0 &#x;) (no match)
T(@0 &#X;) (no match)
T(@0 &#X) (no match)
T(@0 &#11141114111;) 14:�
T(@0 &#x10FFFF0000;) 14:�
T(@0 &#x80;) 6:€
T(@0 &#x8d;) 6:
T(@0 &#xD800;) 8:�
T(@0 &#xDD70;) 8:�
T(@0 &#x1f170;) 9:🅰
A(@0 &amp;=) 5:&
A(@0 &amp=) (no match)
T(@0 &amp=) 4:&
A(@0 &centerdot) (no match)
T(@0 &centerdot) 5:¢
T(@5 &amp&&amp&&) 4:&
T(@5 &amp&&amp;&&) 5:&
A(@5 &amp&&amp&&) 4:&
A(@5 &amp&&amp;&&) 5:&
T(@5 &amp&&amp=&) 4:&
T(@5 &amp&&amp=&) 4:&
T(@5 &amp&&amp/&) 4:&
(done)