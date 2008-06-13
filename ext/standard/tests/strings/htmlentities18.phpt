--TEST--
htmlentities() / htmlspecialchars() "don't double encode" flag support
--FILE--
<?php
$tests = array(
	"abc",
	"abc&amp;sfdsa",
	"test&#043;s &amp; some more &#68;",
	"&; &amp &#a; &9;",
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
unicode(3) "abc"
unicode(3) "abc"
unicode(13) "abc&amp;sfdsa"
unicode(13) "abc&amp;sfdsa"
unicode(33) "test&#043;s &amp; some more &#68;"
unicode(33) "test&#043;s &amp; some more &#68;"
unicode(24) "&; &amp;amp &amp;#a; &9;"
unicode(24) "&; &amp;amp &amp;#a; &9;"
unicode(32) "&amp;kffjadfdhsjfhjasdhffasdfas;"
unicode(32) "&amp;kffjadfdhsjfhjasdhffasdfas;"
unicode(16) "&amp;#8787978789"
unicode(16) "&amp;#8787978789"
unicode(5) "&amp;"
unicode(5) "&amp;"
unicode(15) "&amp;&amp;&amp;"
unicode(15) "&amp;&amp;&amp;"
unicode(17) "&amp;ab&amp;&amp;"
unicode(17) "&amp;ab&amp;&amp;"
