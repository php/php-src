--TEST--
XML Serialization formatting
--EXTENSIONS--
dom
--FILE--
<?php

$tests = [
    '<root></root>',
    '<root> </root>',
    '<!DOCTYPE html><html><head/></html>',
    '<root><child>text</child><!-- comment --><foo>&amp;</foo></root>',
    '<root><child>text</child><![CDATA[x]]><foo><bar/></foo></root>',
    '<root><a/><foo><bar><?foo <x><y/></x> ?><baz/></bar></foo></root>',
];

foreach ($tests as $test) {
    echo "---\n";
    $dom = DOM\XMLDocument::createFromString($test);
    $dom->formatOutput = true;
    echo $dom->saveXML(), "\n";
}

?>
--EXPECT--
---
<?xml version="1.0" encoding="UTF-8"?>
<root/>
---
<?xml version="1.0" encoding="UTF-8"?>
<root> </root>
---
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html>
  <head/>
</html>
---
<?xml version="1.0" encoding="UTF-8"?>
<root>
  <child>text</child>
  <!-- comment -->
  <foo>&amp;</foo>
</root>
---
<?xml version="1.0" encoding="UTF-8"?>
<root><child>text</child><![CDATA[x]]><foo><bar/></foo></root>
---
<?xml version="1.0" encoding="UTF-8"?>
<root>
  <a/>
  <foo>
    <bar>
      <?foo <x><y/></x> ?>
      <baz/>
    </bar>
  </foo>
</root>
