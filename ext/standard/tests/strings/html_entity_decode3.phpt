--TEST--
html_entity_decode: Do not decode numerical entities that refer to non-SGML or otherwise disallowed chars
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
);

echo "*** HTML 4.01  ***\n";

foreach ($tests as $t) {
    $dec = html_entity_decode($t, ENT_QUOTES | ENT_HTML401, "UTF-8");
    if ($t == $dec) {
        echo "$t\tNOT DECODED\n";
    } else {
        echo "$t\tDECODED\n";
    }
}

echo "\n*** XHTML 1.0  ***\n";

foreach ($tests as $t) {
    $dec = html_entity_decode($t, ENT_QUOTES | ENT_XHTML, "UTF-8");
    if ($t == $dec) {
        echo "$t\tNOT DECODED\n";
    } else {
        echo "$t\tDECODED\n";
    }
}

echo "\n*** HTML5  ***\n";

foreach ($tests as $t) {
    $dec = html_entity_decode($t, ENT_QUOTES | ENT_HTML5, "UTF-8");
    if ($t == $dec) {
        echo "$t\tNOT DECODED\n";
    } else {
        echo "$t\tDECODED\n";
    }
}

echo "\n*** XML 1.0  ***\n";

foreach ($tests as $t) {
    $dec = html_entity_decode($t, ENT_QUOTES | ENT_XML1, "UTF-8");
    if ($t == $dec) {
        echo "$t\tNOT DECODED\n";
    } else {
        echo "$t\tDECODED\n";
    }
}

echo "\nDone.\n";
--EXPECT--
*** HTML 4.01  ***
&#0;	NOT DECODED
&#1;	NOT DECODED
&#x09;	DECODED
&#x0A;	DECODED
&#x0B;	NOT DECODED
&#x0C;	NOT DECODED
&#x0D;	DECODED
&#x0E;	NOT DECODED
&#x1F;	NOT DECODED
&#x20;	DECODED
&#x7F;	NOT DECODED
&#x80;	NOT DECODED
&#x9F;	NOT DECODED
&#xA0;	DECODED
&#xD7FF;	DECODED
&#xD800;	NOT DECODED
&#xDFFF;	NOT DECODED
&#xE000;	DECODED
&#xFFFE;	DECODED
&#xFFFF;	DECODED
&#xFDCF;	DECODED
&#xFDD0;	DECODED
&#xFDEF;	DECODED
&#xFDF0;	DECODED
&#x2FFFE;	DECODED
&#x2FFFF;	DECODED

*** XHTML 1.0  ***
&#0;	NOT DECODED
&#1;	NOT DECODED
&#x09;	DECODED
&#x0A;	DECODED
&#x0B;	NOT DECODED
&#x0C;	NOT DECODED
&#x0D;	DECODED
&#x0E;	NOT DECODED
&#x1F;	NOT DECODED
&#x20;	DECODED
&#x7F;	DECODED
&#x80;	DECODED
&#x9F;	DECODED
&#xA0;	DECODED
&#xD7FF;	DECODED
&#xD800;	NOT DECODED
&#xDFFF;	NOT DECODED
&#xE000;	DECODED
&#xFFFE;	NOT DECODED
&#xFFFF;	NOT DECODED
&#xFDCF;	DECODED
&#xFDD0;	DECODED
&#xFDEF;	DECODED
&#xFDF0;	DECODED
&#x2FFFE;	DECODED
&#x2FFFF;	DECODED

*** HTML5  ***
&#0;	NOT DECODED
&#1;	NOT DECODED
&#x09;	DECODED
&#x0A;	DECODED
&#x0B;	NOT DECODED
&#x0C;	DECODED
&#x0D;	NOT DECODED
&#x0E;	NOT DECODED
&#x1F;	NOT DECODED
&#x20;	DECODED
&#x7F;	NOT DECODED
&#x80;	NOT DECODED
&#x9F;	NOT DECODED
&#xA0;	DECODED
&#xD7FF;	DECODED
&#xD800;	NOT DECODED
&#xDFFF;	NOT DECODED
&#xE000;	DECODED
&#xFFFE;	NOT DECODED
&#xFFFF;	NOT DECODED
&#xFDCF;	DECODED
&#xFDD0;	NOT DECODED
&#xFDEF;	NOT DECODED
&#xFDF0;	DECODED
&#x2FFFE;	NOT DECODED
&#x2FFFF;	NOT DECODED

*** XML 1.0  ***
&#0;	NOT DECODED
&#1;	NOT DECODED
&#x09;	DECODED
&#x0A;	DECODED
&#x0B;	NOT DECODED
&#x0C;	NOT DECODED
&#x0D;	DECODED
&#x0E;	NOT DECODED
&#x1F;	NOT DECODED
&#x20;	DECODED
&#x7F;	DECODED
&#x80;	DECODED
&#x9F;	DECODED
&#xA0;	DECODED
&#xD7FF;	DECODED
&#xD800;	NOT DECODED
&#xDFFF;	NOT DECODED
&#xE000;	DECODED
&#xFFFE;	NOT DECODED
&#xFFFF;	NOT DECODED
&#xFDCF;	DECODED
&#xFDD0;	DECODED
&#xFDEF;	DECODED
&#xFDF0;	DECODED
&#x2FFFE;	DECODED
&#x2FFFF;	DECODED

Done.
