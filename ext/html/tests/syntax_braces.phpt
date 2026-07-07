--TEST--
Markup syntax: literal braces via character references or {'{'} (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
// { begins interpolation; a literal { is written as in HTML or JSX.

// Character references decode to literal braces at compile time (no interpolation).
echo (<p>&#123;x&#125;</p>)->__toString(), "\n";
echo (<p>&lbrace;x&rbrace;</p>)->__toString(), "\n";

// The JSX form: an interpolated string literal.
echo (<p>{'{'}x{'}'}</p>)->__toString(), "\n";

// A lone } in text is already literal.
echo (<p>a } b</p>)->__toString(), "\n";

// Literal JSON in a <script> uses the raw() island pattern (text children are
// HTML-escaped, and browsers never entity-decode inside raw-text elements).
echo (<script type="application/json" id="config">{Html\raw(<<<'JSON'
{"@type": "Article", "title": "Braces & quotes stay verbatim"}
JSON)}</script>)->__toString(), "\n";
?>
--EXPECT--
<p>{x}</p>
<p>{x}</p>
<p>{x}</p>
<p>a } b</p>
<script type="application/json" id="config">{"@type": "Article", "title": "Braces & quotes stay verbatim"}</script>
