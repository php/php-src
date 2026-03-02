--TEST--
DOMDocument::getElementsByTagNameNS() match any namespace
--EXTENSIONS--
dom
--FILE--
<?php

/* Sample document taken from https://www.php.net/manual/en/domdocument.getelementsbytagname.php */
$xml = <<<EOD
<?xml version="1.0" ?>
<chapter xmlns:xi="http://www.w3.org/2001/XInclude">
<title>Books of the other guy..</title>
<para>
 <xi:include href="book.xml">
  <xi:fallback>
   <error>xinclude: book.xml not found</error>
  </xi:fallback>
 </xi:include>
 <include>
  This is another namespace
 </include>
</para>
</chapter>
EOD;
$dom = new DOMDocument;

// load the XML string defined above
$dom->loadXML($xml);

function test($namespace, $local) {
    global $dom;
    $namespace_str = $namespace !== NULL ? "'$namespace'" : "null";
    echo "-- getElementsByTagNameNS($namespace_str, '$local') --\n";
    foreach ($dom->getElementsByTagNameNS($namespace, $local) as $element) {
        echo 'local name: \'', $element->localName, '\', prefix: \'', $element->prefix, "'\n";
    }
}

// Should *also* include objects even without a namespace
test(null, '*');
// Should *also* include objects even without a namespace
test('*', '*');
// Should *only* include objects without a namespace
test('', '*');
// Should *only* include objects with the specified namespace
test('http://www.w3.org/2001/XInclude', '*');
// Should not give any output
test('', 'fallback');
// Should not give any output, because the null namespace is the same as the empty namespace
test(null, 'fallback');
// Should only output the include from the empty namespace
test(null, 'include');

?>
--EXPECT--
-- getElementsByTagNameNS(null, '*') --
local name: 'chapter', prefix: ''
local name: 'title', prefix: ''
local name: 'para', prefix: ''
local name: 'error', prefix: ''
local name: 'include', prefix: ''
-- getElementsByTagNameNS('*', '*') --
local name: 'chapter', prefix: ''
local name: 'title', prefix: ''
local name: 'para', prefix: ''
local name: 'include', prefix: 'xi'
local name: 'fallback', prefix: 'xi'
local name: 'error', prefix: ''
local name: 'include', prefix: ''
-- getElementsByTagNameNS('', '*') --
local name: 'chapter', prefix: ''
local name: 'title', prefix: ''
local name: 'para', prefix: ''
local name: 'error', prefix: ''
local name: 'include', prefix: ''
-- getElementsByTagNameNS('http://www.w3.org/2001/XInclude', '*') --
local name: 'include', prefix: 'xi'
local name: 'fallback', prefix: 'xi'
-- getElementsByTagNameNS('', 'fallback') --
-- getElementsByTagNameNS(null, 'fallback') --
-- getElementsByTagNameNS(null, 'include') --
local name: 'include', prefix: ''
