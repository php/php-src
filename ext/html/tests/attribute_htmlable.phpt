--TEST--
Htmlable attribute values (Html\raw, Html\escape) pass through without double-escaping
--EXTENSIONS--
html
--FILE--
<?php
// Html\raw(): trusted content, emitted verbatim (the entity survives).
echo <span title={\Html\raw('&nbsp;')}>x</span>, "\n";

// Html\escape(): escaped exactly once, not re-escaped at render time.
echo <span title={\Html\escape('a & b')}>x</span>, "\n";

// Plain strings still escape by default.
echo <span title={'a & b'}>x</span>, "\n";

// A generic Stringable (not Htmlable) still escapes.
class S { public function __toString(): string { return 'x & y'; } }
echo <span title={new S}>x</span>, "\n";
?>
--EXPECT--
<span title="&nbsp;">x</span>
<span title="a &amp; b">x</span>
<span title="a &amp; b">x</span>
<span title="x &amp; y">x</span>
