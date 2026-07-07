--TEST--
Html\Fragment, void-element serialization, and raw()/escape() helpers
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element;
use Html\Fragment;
use function Html\raw;
use function Html\escape;

// Fragment: no wrapper element, children concatenated.
$frag = new Fragment([
    new Element('h1', [], ['Title']),
    new Element('p', [], ['Body']),
]);
echo $frag->__toString(), "\n";

// Void elements emit clean HTML5 (no slash, no closing tag).
echo (new Element('br'))->__toString(), "\n";
echo (new Element('img', ['src' => '/a.png', 'alt' => 'A & B']))->__toString(), "\n";

// raw(): trusted passthrough, not escaped.
echo raw('<b>x</b>')->__toString(), "\n";

// escape(): escaped once, then safe to embed without double-escaping.
$safe = escape('<b>x</b>');
echo $safe->__toString(), "\n";
echo (new Element('p', [], [$safe]))->__toString(), "\n"; // not escaped again

var_dump(raw('y') instanceof Html\Htmlable);
?>
--EXPECT--
<h1>Title</h1><p>Body</p>
<br>
<img src="/a.png" alt="A &amp; B">
<b>x</b>
&lt;b&gt;x&lt;/b&gt;
<p>&lt;b&gt;x&lt;/b&gt;</p>
bool(true)
