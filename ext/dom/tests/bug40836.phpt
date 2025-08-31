--TEST--
Bug #40836 (Segfault in insertBefore)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument("1.0", "UTF-8");
$dom->preserveWhiteSpace = false;
$xml = '<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
  <entry xmlns="http://www.w3.org/2005/Atom">
    <updated>2007-02-14T00:00:00+01:00</updated>
    <content>
      <div xmlns="http://www.w3.org/1999/xhtml">
        <p>paragraph</p>
      </div>
    </content>
  </entry>
</feed>';
$dom->loadXML($xml);
$entry = $dom->getElementsByTagNameNS("http://www.w3.org/2005/Atom", "entry")->item(0);
$contentNode = $entry->getElementsByTagName("content")->item(0)->firstChild;
$dateNode = $entry->getElementsByTagName("updated")->item(0)->firstChild;
$contentNode->firstChild->insertBefore($dateNode);
echo $dom->saveXML();
?>
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom"><entry xmlns="http://www.w3.org/2005/Atom"><updated/><content><div xmlns="http://www.w3.org/1999/xhtml"><p>paragraph2007-02-14T00:00:00+01:00</p></div></content></entry></feed>
