--TEST--
Html: dynamic tag/attribute names are validated so they can't break out (security)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;

function err(callable $fn): string {
    try { return $fn()->__toString(); }
    catch (\Throwable $e) { return get_class($e) . ': ' . $e->getMessage(); }
}

// Attribute name from an attacker-controlled spread key - must be rejected.
echo err(fn() => new E('a', ['x" onmouseover="alert(1)' => 'y'], ['hi'])), "\n";

// Tag name from a dynamic source - must be rejected.
echo err(fn() => new E('a><script>alert(1)</script>', [], [])), "\n";

// Empty names rejected.
echo err(fn() => new E('', [], [])), "\n";
echo err(fn() => new E('div', ['' => 'x'], [])), "\n";

// Legitimate dynamic names still work (custom elements, data-/aria-, namespaces).
echo (new E('my-widget', ['data-x' => '1', 'aria-label' => 'ok', 'xml:lang' => 'en'], ['x']))->__toString(), "\n";
echo (<a {...['data-id' => 5]}>ok</a>), "\n";
?>
--EXPECTF--
ValueError: Invalid attribute name "x" onmouseover="alert(1)"
ValueError: Invalid tag name "a><script>alert(1)</script>"
ValueError: Invalid tag name ""
ValueError: Invalid attribute name ""
<my-widget data-x="1" aria-label="ok" xml:lang="en">x</my-widget>
<a data-id="5">ok</a>
