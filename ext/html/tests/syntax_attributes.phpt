--TEST--
Markup syntax: attributes (literal, {expr}, bare boolean) with coercion (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
$url = "/x?a=1&b=2";
$id = 42;
$on = true;
$off = false;
$missing = null;

// literal value, {expr} value, int, bool true/false, null, bare boolean
echo (<a href={$url} class="btn primary" data-id={$id} disabled={$on} hidden={$off} title={$missing} required>link</a>)->__toString(), "\n";

// attribute value is escaped
echo (<div title="a & b < c">x</div>)->__toString(), "\n";

// self-closing element with attributes
echo (<img src={$url} alt="A & B"/>)->__toString(), "\n";
?>
--EXPECT--
<a href="/x?a=1&amp;b=2" class="btn primary" data-id="42" disabled required>link</a>
<div title="a &amp; b &lt; c">x</div>
<img src="/x?a=1&amp;b=2" alt="A &amp; B">
