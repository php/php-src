--TEST--
htmlentities() / htmlspecialchars() ENT_DISALLOWED with entities and no double encode
--FILE--
<?php
$tests = array(
    "&#0;", //C0
    "&#1;",
    "&#x09;",
    "&#x0A;",
    "&#x0B;",
    "&#x0C;",
    "&#x0D;", //note that HTML5 is unique in that it forbids this entity, but allows a literal U+0D
    "&#x0E;",
    "&#x1F;",
    "&#x20;", //allowed always
    "&#x7F;", //DEL
    "&#x80;", //C1
    "&#x9F;",
    "&#xA0;", //allowed always
    "&#xD7FF;", //surrogates
    "&#xD800;",
    "&#xDFFF;",
    "&#xE000;", //allowed always
    "&#xFFFE;", //nonchar
    "&#xFFFF;",
    "&#xFDCF;", //allowed always
    "&#xFDD0;", //nonchar
    "&#xFDEF;",
    "&#xFDF0;", //allowed always
    "&#x2FFFE;", //nonchar
    "&#x2FFFF;",
    "&#x110000;", //bad reference
);

function test($flag, $flag2=ENT_DISALLOWED, $charset="UTF-8") {
    global $tests;
    $i = -1;
    error_reporting(-1 & ~E_STRICT);
    foreach ($tests as $test) {
        $i++;
        $a = htmlentities($test, $flag | $flag2, $charset, FALSE);
        $b = htmlspecialchars($test, $flag | $flag2, $charset, FALSE);
        
        if ($a == $b)
            echo sprintf("%s\t%s", $test, $a==$test?"NOT CHANGED":"CHANGED"), "\n";
        else
            echo sprintf("%s\tCHANGED (%s, %s)", $test, $a, $b), "\n";
    }
    error_reporting(-1);
}

echo "*** Testing HTML 4.01 ***\n";

test(ENT_HTML401);

echo "\n*** Testing XHTML 1.0 ***\n";

test(ENT_XHTML);

echo "\n*** Testing HTML 5 ***\n";

test(ENT_HTML5);

echo "\n*** Testing XML 1.0 ***\n";

test(ENT_XML1);

echo "\n*** Testing 5 without the flag ***\n";

test(ENT_HTML5, 0);

echo "\n*** Testing HTML 5 with another single-byte encoding ***\n";

test(ENT_HTML5, ENT_DISALLOWED, "Windows-1251");

echo "\n*** Testing HTML 5 with another multibyte-byte encoding ***\n";

test(ENT_HTML5, ENT_DISALLOWED, "SJIS");

?>
--EXPECT--
*** Testing HTML 4.01 ***
&#0;	NOT CHANGED
&#1;	NOT CHANGED
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	NOT CHANGED
&#x0C;	NOT CHANGED
&#x0D;	NOT CHANGED
&#x0E;	NOT CHANGED
&#x1F;	NOT CHANGED
&#x20;	NOT CHANGED
&#x7F;	NOT CHANGED
&#x80;	NOT CHANGED
&#x9F;	NOT CHANGED
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	NOT CHANGED
&#xDFFF;	NOT CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	NOT CHANGED
&#xFFFF;	NOT CHANGED
&#xFDCF;	NOT CHANGED
&#xFDD0;	NOT CHANGED
&#xFDEF;	NOT CHANGED
&#xFDF0;	NOT CHANGED
&#x2FFFE;	NOT CHANGED
&#x2FFFF;	NOT CHANGED
&#x110000;	CHANGED

*** Testing XHTML 1.0 ***
&#0;	CHANGED
&#1;	CHANGED
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	CHANGED
&#x0C;	CHANGED
&#x0D;	NOT CHANGED
&#x0E;	CHANGED
&#x1F;	CHANGED
&#x20;	NOT CHANGED
&#x7F;	NOT CHANGED
&#x80;	NOT CHANGED
&#x9F;	NOT CHANGED
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	CHANGED
&#xDFFF;	CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	CHANGED
&#xFFFF;	CHANGED
&#xFDCF;	NOT CHANGED
&#xFDD0;	NOT CHANGED
&#xFDEF;	NOT CHANGED
&#xFDF0;	NOT CHANGED
&#x2FFFE;	NOT CHANGED
&#x2FFFF;	NOT CHANGED
&#x110000;	CHANGED

*** Testing HTML 5 ***
&#0;	CHANGED (&amp;&num;0&semi;, &amp;#0;)
&#1;	CHANGED (&amp;&num;1&semi;, &amp;#1;)
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	CHANGED (&amp;&num;x0B&semi;, &amp;#x0B;)
&#x0C;	NOT CHANGED
&#x0D;	CHANGED (&amp;&num;x0D&semi;, &amp;#x0D;)
&#x0E;	CHANGED (&amp;&num;x0E&semi;, &amp;#x0E;)
&#x1F;	CHANGED (&amp;&num;x1F&semi;, &amp;#x1F;)
&#x20;	NOT CHANGED
&#x7F;	CHANGED (&amp;&num;x7F&semi;, &amp;#x7F;)
&#x80;	CHANGED (&amp;&num;x80&semi;, &amp;#x80;)
&#x9F;	CHANGED (&amp;&num;x9F&semi;, &amp;#x9F;)
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	NOT CHANGED
&#xDFFF;	NOT CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	CHANGED (&amp;&num;xFFFE&semi;, &amp;#xFFFE;)
&#xFFFF;	CHANGED (&amp;&num;xFFFF&semi;, &amp;#xFFFF;)
&#xFDCF;	NOT CHANGED
&#xFDD0;	CHANGED (&amp;&num;xFDD0&semi;, &amp;#xFDD0;)
&#xFDEF;	CHANGED (&amp;&num;xFDEF&semi;, &amp;#xFDEF;)
&#xFDF0;	NOT CHANGED
&#x2FFFE;	CHANGED (&amp;&num;x2FFFE&semi;, &amp;#x2FFFE;)
&#x2FFFF;	CHANGED (&amp;&num;x2FFFF&semi;, &amp;#x2FFFF;)
&#x110000;	CHANGED (&amp;&num;x110000&semi;, &amp;#x110000;)

*** Testing XML 1.0 ***
&#0;	CHANGED
&#1;	CHANGED
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	CHANGED
&#x0C;	CHANGED
&#x0D;	NOT CHANGED
&#x0E;	CHANGED
&#x1F;	CHANGED
&#x20;	NOT CHANGED
&#x7F;	NOT CHANGED
&#x80;	NOT CHANGED
&#x9F;	NOT CHANGED
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	CHANGED
&#xDFFF;	CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	CHANGED
&#xFFFF;	CHANGED
&#xFDCF;	NOT CHANGED
&#xFDD0;	NOT CHANGED
&#xFDEF;	NOT CHANGED
&#xFDF0;	NOT CHANGED
&#x2FFFE;	NOT CHANGED
&#x2FFFF;	NOT CHANGED
&#x110000;	CHANGED

*** Testing 5 without the flag ***
&#0;	NOT CHANGED
&#1;	NOT CHANGED
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	NOT CHANGED
&#x0C;	NOT CHANGED
&#x0D;	NOT CHANGED
&#x0E;	NOT CHANGED
&#x1F;	NOT CHANGED
&#x20;	NOT CHANGED
&#x7F;	NOT CHANGED
&#x80;	NOT CHANGED
&#x9F;	NOT CHANGED
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	NOT CHANGED
&#xDFFF;	NOT CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	NOT CHANGED
&#xFFFF;	NOT CHANGED
&#xFDCF;	NOT CHANGED
&#xFDD0;	NOT CHANGED
&#xFDEF;	NOT CHANGED
&#xFDF0;	NOT CHANGED
&#x2FFFE;	NOT CHANGED
&#x2FFFF;	NOT CHANGED
&#x110000;	CHANGED (&amp;&num;x110000&semi;, &amp;#x110000;)

*** Testing HTML 5 with another single-byte encoding ***
&#0;	CHANGED (&amp;&num;0&semi;, &amp;#0;)
&#1;	CHANGED (&amp;&num;1&semi;, &amp;#1;)
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	CHANGED (&amp;&num;x0B&semi;, &amp;#x0B;)
&#x0C;	NOT CHANGED
&#x0D;	CHANGED (&amp;&num;x0D&semi;, &amp;#x0D;)
&#x0E;	CHANGED (&amp;&num;x0E&semi;, &amp;#x0E;)
&#x1F;	CHANGED (&amp;&num;x1F&semi;, &amp;#x1F;)
&#x20;	NOT CHANGED
&#x7F;	CHANGED (&amp;&num;x7F&semi;, &amp;#x7F;)
&#x80;	CHANGED (&amp;&num;x80&semi;, &amp;#x80;)
&#x9F;	CHANGED (&amp;&num;x9F&semi;, &amp;#x9F;)
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	NOT CHANGED
&#xDFFF;	NOT CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	CHANGED (&amp;&num;xFFFE&semi;, &amp;#xFFFE;)
&#xFFFF;	CHANGED (&amp;&num;xFFFF&semi;, &amp;#xFFFF;)
&#xFDCF;	NOT CHANGED
&#xFDD0;	CHANGED (&amp;&num;xFDD0&semi;, &amp;#xFDD0;)
&#xFDEF;	CHANGED (&amp;&num;xFDEF&semi;, &amp;#xFDEF;)
&#xFDF0;	NOT CHANGED
&#x2FFFE;	CHANGED (&amp;&num;x2FFFE&semi;, &amp;#x2FFFE;)
&#x2FFFF;	CHANGED (&amp;&num;x2FFFF&semi;, &amp;#x2FFFF;)
&#x110000;	CHANGED (&amp;&num;x110000&semi;, &amp;#x110000;)

*** Testing HTML 5 with another multibyte-byte encoding ***
&#0;	CHANGED
&#1;	CHANGED
&#x09;	NOT CHANGED
&#x0A;	NOT CHANGED
&#x0B;	CHANGED
&#x0C;	NOT CHANGED
&#x0D;	CHANGED
&#x0E;	CHANGED
&#x1F;	CHANGED
&#x20;	NOT CHANGED
&#x7F;	CHANGED
&#x80;	CHANGED
&#x9F;	CHANGED
&#xA0;	NOT CHANGED
&#xD7FF;	NOT CHANGED
&#xD800;	NOT CHANGED
&#xDFFF;	NOT CHANGED
&#xE000;	NOT CHANGED
&#xFFFE;	CHANGED
&#xFFFF;	CHANGED
&#xFDCF;	NOT CHANGED
&#xFDD0;	CHANGED
&#xFDEF;	CHANGED
&#xFDF0;	NOT CHANGED
&#x2FFFE;	CHANGED
&#x2FFFF;	CHANGED
&#x110000;	CHANGED
