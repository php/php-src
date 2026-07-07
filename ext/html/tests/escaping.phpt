--TEST--
Html\Element escapes string children and attribute values by default
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element;

// Child text is escaped.
echo (new Element('p', [], ['Tom & Jerry <3 "quotes" \'apostrophe\'']))->__toString(), "\n";

// Attribute values are escaped.
echo (new Element('div', ['title' => 'a "b" & <c>'], []))->__toString(), "\n";

// A classic XSS attempt is neutralised.
$evil = '<script>alert(1)</script>';
echo (new Element('span', [], [$evil]))->__toString(), "\n";
?>
--EXPECT--
<p>Tom &amp; Jerry &lt;3 &quot;quotes&quot; &#039;apostrophe&#039;</p>
<div title="a &quot;b&quot; &amp; &lt;c&gt;"></div>
<span>&lt;script&gt;alert(1)&lt;/script&gt;</span>
