--TEST--
Markup syntax: HTML character references decode in text and attribute values (RFC §7)
--EXTENSIONS--
html
--FILE--
<?php
// Named entities decode to their UTF-8 text; the ampersand round-trips:
// "&amp;" in source is "&" in the value, re-escaped to "&amp;" on output.
echo <p>Fish &amp; chips &mdash; &pound;5</p>, "\n";

// Case matters, exactly as in HTML: &Aacute; and &aacute; differ.
echo <p>&Aacute; vs &aacute;</p>, "\n";

// Numeric references: decimal, hex (either x/X case), multi-codepoint named.
echo <p>&#65;&#x42;&#X43; &#x1F600; &nGt;</p>, "\n";

// &nbsp; is a real U+00A0, not the entity text (htmlspecialchars leaves it alone).
var_dump((string)<i>&nbsp;</i> === "<i>\u{00A0}</i>");

// Lenient like HTML/JSX: unknown names, bare "&", missing semicolons,
// and invalid numerics (zero, surrogate, out of range, empty) stay literal
// and are escaped on output.
echo <p>a & b &nosuchentity; &amp c</p>, "\n";
echo <p>&#0; &#xD800; &#x110000; &#; &#x; &#xG;</p>, "\n";

// Attribute values decode too; the "&" re-escapes when serialized.
echo <a title="Caf&eacute; &amp; bar" data-x="&#x2764;">x</a>, "\n";

// lt/gt decode to real angle brackets in the value, escaped on output --
// so entities cannot smuggle markup structure.
echo <p>&lt;script&gt;alert(1)&lt;/script&gt;</p>, "\n";

// Entities decode after whitespace normalization, so &#32; survives where a
// literal trailing space would be trimmed (the entity spelling of JSX {' '}).
echo <p>a&#32;
</p>, "\n";

// Entities inside <!-- comments --> stay verbatim (comments are raw output).
echo <div><!-- &amp; &#65; --></div>, "\n";

// Interpolated strings are never entity-decoded; they are plain PHP values.
echo <p>{"&amp;"}</p>, "\n";
?>
--EXPECT--
<p>Fish &amp; chips — £5</p>
<p>Á vs á</p>
<p>ABC 😀 ≫⃒</p>
bool(true)
<p>a &amp; b &amp;nosuchentity; &amp;amp c</p>
<p>&amp;#0; &amp;#xD800; &amp;#x110000; &amp;#; &amp;#x; &amp;#xG;</p>
<a title="Café &amp; bar" data-x="❤">x</a>
<p>&lt;script&gt;alert(1)&lt;/script&gt;</p>
<p>a </p>
<div><!-- &amp; &#65; --></div>
<p>&amp;amp;</p>
