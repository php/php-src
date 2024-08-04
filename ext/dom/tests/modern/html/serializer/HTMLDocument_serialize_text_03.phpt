--TEST--
Dom\HTMLDocument serialization escape text 03 - special tags in namespace should encode content
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$body = $dom->appendChild($dom->createElement("body"));
foreach (["style", "script", "xmp", "iframe", "noembed", "noframes", "plaintext", "noscript"] as $tag) {
    $tag = $body->appendChild($dom->createElementNS("some:ns", $tag));
    $tag->textContent = "&\"<>\xc2\xa0 foobar";
    $body->append("\n");
}
echo $dom->saveHtml();

?>
--EXPECT--
<body><style>&amp;"&lt;&gt;&nbsp; foobar</style>
<script>&amp;"&lt;&gt;&nbsp; foobar</script>
<xmp>&amp;"&lt;&gt;&nbsp; foobar</xmp>
<iframe>&amp;"&lt;&gt;&nbsp; foobar</iframe>
<noembed>&amp;"&lt;&gt;&nbsp; foobar</noembed>
<noframes>&amp;"&lt;&gt;&nbsp; foobar</noframes>
<plaintext>&amp;"&lt;&gt;&nbsp; foobar</plaintext>
<noscript>&amp;"&lt;&gt;&nbsp; foobar</noscript>
</body>
