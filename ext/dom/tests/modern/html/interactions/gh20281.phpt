--TEST--
GH-20281 (\Dom\Document::getElementById() is inconsistent after nodes are removed)
--EXTENSIONS--
dom
--CREDITS--
cscott
--FILE--
<?php
$d = \Dom\HTMLDocument::createFromString('<p id="a">b</p><p id="a">c</p>', LIBXML_NOERROR);
$p = $d->getElementById('a');
$p->remove();
echo $d->getElementById('a')->textContent, "\n";
?>
--EXPECT--
c
