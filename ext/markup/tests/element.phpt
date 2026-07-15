--TEST--
Markup\Element/Fragment/Raw construction API: rendering, escaping, attribute coercion, Html attributes, void elements, raw()/escape(), name validation
--EXTENSIONS--
markup
--FILE--
<?php
use Markup\Element;
use Markup\Element as E;
use Markup\Fragment;
use function Markup\raw;
use function Markup\escape;

// --- basic rendering ---

$el = new Element('a', ['href' => 'https://php.net', 'class' => 'btn'], ['Click me']);
echo $el->__toString(), PHP_EOL;
echo $el, PHP_EOL; // echo casts via __toString

var_dump($el instanceof Markup\Html);
var_dump($el instanceof Stringable);
var_dump($el->tag);
var_dump($el->attributes);
var_dump($el->children);

// --- escaping ---

// Child text is escaped.
echo (new Element('p', [], ['Tom & Jerry <3 "quotes" \'apostrophe\'']))->__toString(), PHP_EOL;

// Attribute values are escaped.
echo (new Element('div', ['title' => 'a "b" & <c>'], []))->__toString(), PHP_EOL;

// A classic XSS attempt is neutralised.
$evil = '<script>alert(1)</script>';
echo (new Element('span', [], [$evil]))->__toString(), PHP_EOL;

// --- attribute value coercion ---

// null and false omit the attribute; true is a bare boolean attribute.
echo (new Element('input', [
    'type' => 'checkbox',
    'checked' => true,
    'disabled' => false,
    'value' => null,
    'tabindex' => 3,
    'data-ratio' => 1.5,
]))->__toString(), PHP_EOL;

// Stringable attribute value.
$s = new class implements Stringable {
    public function __toString(): string { return 'x&y'; }
};
echo (new Element('a', ['data-x' => $s]))->__toString(), PHP_EOL;

// Arrays are reserved -> TypeError.
try {
    (new Element('div', ['class' => ['a', 'b']]))->__toString();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

// --- Html attribute values (no double-escaping) ---

// Markup\raw(): trusted content, emitted verbatim (the entity survives).
echo <span title={\Markup\raw('&nbsp;')}>x</span>, PHP_EOL;

// Markup\escape(): escaped exactly once, not re-escaped at render time.
echo <span title={\Markup\escape('a & b')}>x</span>, PHP_EOL;

// Plain strings still escape by default.
echo <span title={'a & b'}>x</span>, PHP_EOL;

// A generic Stringable (not Html) still escapes.
class S { public function __toString(): string { return 'x & y'; } }
echo <span title={new S}>x</span>, PHP_EOL;

// --- Fragment, void elements, raw()/escape() helpers ---

// Fragment: no wrapper element, children concatenated.
$frag = new Fragment([
    new Element('h1', [], ['Title']),
    new Element('p', [], ['Body']),
]);
echo $frag->__toString(), PHP_EOL;

// Void elements emit clean HTML5 (no slash, no closing tag).
echo (new Element('br'))->__toString(), PHP_EOL;
echo (new Element('img', ['src' => '/a.png', 'alt' => 'A & B']))->__toString(), PHP_EOL;

// raw(): trusted passthrough, not escaped.
echo raw('<b>x</b>')->__toString(), PHP_EOL;

// escape(): escaped once, then safe to embed without double-escaping.
$safe = escape('<b>x</b>');
echo $safe->__toString(), PHP_EOL;
echo (new Element('p', [], [$safe]))->__toString(), PHP_EOL; // not escaped again

var_dump(raw('y') instanceof Markup\Html);

// --- tag/attribute-name validation (security) ---

function err(callable $fn): string {
    try { return $fn()->__toString(); }
    catch (\Throwable $e) { return $e::class . ': ' . $e->getMessage(); }
}

// Attribute name from an attacker-controlled spread key - must be rejected.
echo err(fn() => new E('a', ['x" onmouseover="alert(1)' => 'y'], ['hi'])), PHP_EOL;

// Tag name from a dynamic source - must be rejected.
echo err(fn() => new E('a><script>alert(1)</script>', [], [])), PHP_EOL;

// Empty names rejected.
echo err(fn() => new E('', [], [])), PHP_EOL;
echo err(fn() => new E('div', ['' => 'x'], [])), PHP_EOL;

// Legitimate dynamic names still work (custom elements, data-/aria-, namespaces).
echo (new E('my-widget', ['data-x' => '1', 'aria-label' => 'ok', 'xml:lang' => 'en'], ['x']))->__toString(), PHP_EOL;
echo (<a {...['data-id' => 5]}>ok</a>), PHP_EOL;
?>
--EXPECTF--
<a href="https://php.net" class="btn">Click me</a>
<a href="https://php.net" class="btn">Click me</a>
bool(true)
bool(true)
string(1) "a"
array(2) {
  ["href"]=>
  string(15) "https://php.net"
  ["class"]=>
  string(3) "btn"
}
array(1) {
  [0]=>
  string(8) "Click me"
}
<p>Tom &amp; Jerry &lt;3 &quot;quotes&quot; &#039;apostrophe&#039;</p>
<div title="a &quot;b&quot; &amp; &lt;c&gt;"></div>
<span>&lt;script&gt;alert(1)&lt;/script&gt;</span>
<input type="checkbox" checked tabindex="3" data-ratio="1.5">
<a data-x="x&amp;y"></a>
TypeError: Attribute "class" value must be of type string|int|float|bool|null|Stringable, array given
<span title="&nbsp;">x</span>
<span title="a &amp; b">x</span>
<span title="a &amp; b">x</span>
<span title="x &amp; y">x</span>
<h1>Title</h1><p>Body</p>
<br>
<img src="/a.png" alt="A &amp; B">
<b>x</b>
&lt;b&gt;x&lt;/b&gt;
<p>&lt;b&gt;x&lt;/b&gt;</p>
bool(true)
ValueError: Invalid attribute name "x" onmouseover="alert(1)"
ValueError: Invalid tag name "a><script>alert(1)</script>"
ValueError: Invalid tag name ""
ValueError: Invalid attribute name ""
<my-widget data-x="1" aria-label="ok" xml:lang="en">x</my-widget>
<a data-id="5">ok</a>
