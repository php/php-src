--TEST--
htmlentities() / htmlspecialchars() "don't double encode" flag support
--FILE--
<?php
$tests = array(
    "abc",
    "abc&amp;sfdsa",
    "test&#043;s &amp; some more &#68;",
    "test&#x2b;s &amp; some more &#X44;",
    "&; &amp &#a; &9; &#xyz;",
    "&kffjadfdhsjfhjasdhffasdfas;",
    "&#8787978789",
    "&",
    "&&amp;&",
    "&ab&amp;&",
);

foreach ($tests as $test) {
    var_dump(htmlentities($test, ENT_QUOTES, NULL, FALSE));
    var_dump(htmlspecialchars($test, ENT_QUOTES, NULL, FALSE));
}
?>
--EXPECT--
string(3) "abc"
string(3) "abc"
string(13) "abc&amp;sfdsa"
string(13) "abc&amp;sfdsa"
string(33) "test&#043;s &amp; some more &#68;"
string(33) "test&#043;s &amp; some more &#68;"
string(34) "test&#x2b;s &amp; some more &#X44;"
string(34) "test&#x2b;s &amp; some more &#X44;"
string(43) "&amp;; &amp;amp &amp;#a; &amp;9; &amp;#xyz;"
string(43) "&amp;; &amp;amp &amp;#a; &amp;9; &amp;#xyz;"
string(32) "&amp;kffjadfdhsjfhjasdhffasdfas;"
string(32) "&amp;kffjadfdhsjfhjasdhffasdfas;"
string(16) "&amp;#8787978789"
string(16) "&amp;#8787978789"
string(5) "&amp;"
string(5) "&amp;"
string(15) "&amp;&amp;&amp;"
string(15) "&amp;&amp;&amp;"
string(17) "&amp;ab&amp;&amp;"
string(17) "&amp;ab&amp;&amp;"
