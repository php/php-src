--TEST--
DOM\HTMLDocument serialization escape text 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$body = $dom->appendChild($dom->createElement("body"));
foreach (["style", "script", "xmp", "iframe", "noembed", "noframes", "plaintext", "noscript"] as $tag) {
    $tag = $body->appendChild($dom->createElement($tag));
    $tag->textContent = "&\"<>\xc2\xa0 foobar";
    $body->appendChild(new DOMText("\n"));
}
echo $dom->saveHTML();

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
