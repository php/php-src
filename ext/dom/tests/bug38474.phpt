--TEST--
Bug #38474 (getAttribute select attribute by order, even when prefixed) (OK to fail with libxml2 < 2.6.2x)
--SKIPIF--
<?php
require_once('skipif.inc');
if (version_compare(LIBXML_DOTTED_VERSION, "2.6.20", "<")) {
    print "skip libxml version " . LIBXML_DOTTED_VERSION;
}
?>
--FILE--
<?php
$xml = '<node xmlns:pre="http://foo.com/tr/pre" 
              xmlns:post="http://foo.com/tr/post"
              pre:type="bar" type="foo" ><sub /></node>';
$dom = new DomDocument();
$dom->loadXML($xml);
echo $dom->firstChild->getAttribute('type')."\n";
echo $dom->firstChild->getAttribute('pre:type')."\n";

$dom->firstChild->setAttribute('pre:type', 'bar2');
$dom->firstChild->setAttribute('type', 'foo2');
$dom->firstChild->setAttribute('post:type', 'baz');
$dom->firstChild->setAttribute('new:type', 'baz2');

echo $dom->firstChild->getAttribute('type')."\n";
echo $dom->firstChild->getAttribute('pre:type')."\n";
echo $dom->firstChild->getAttribute('post:type')."\n";

$dom->firstChild->removeAttribute('pre:type');
$dom->firstChild->removeAttribute('type');

echo $dom->firstChild->getAttribute('type')."\n";
echo $dom->firstChild->getAttribute('pre:type')."\n";
echo $dom->firstChild->getAttribute('post:type')."\n";
echo $dom->firstChild->getAttribute('new:type');
?>
--EXPECT--
foo
bar
foo2
bar2
baz


baz
baz2
