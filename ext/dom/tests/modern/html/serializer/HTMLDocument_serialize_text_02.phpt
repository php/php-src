--TEST--
Dom\HTMLDocument serialization escape text 02 - special tags in html namespace
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$body = $dom->appendChild($dom->createElement("body"));
foreach (["style", "script", "xmp", "iframe", "noembed", "noframes", "plaintext", "noscript"] as $tag) {
    $tag = $body->appendChild($dom->createElement($tag));
    $tag->textContent = "&\"<>\xc2\xa0 foobar";
    $body->append("\n");
}
echo $dom->saveHtml();

?>
--EXPECT--
<body><style>&"<>  foobar</style>
<script>&"<>  foobar</script>
<xmp>&"<>  foobar</xmp>
<iframe>&"<>  foobar</iframe>
<noembed>&"<>  foobar</noembed>
<noframes>&"<>  foobar</noframes>
<plaintext>&"<>  foobar</plaintext>
<noscript>&amp;"&lt;&gt;&nbsp; foobar</noscript>
</body>
