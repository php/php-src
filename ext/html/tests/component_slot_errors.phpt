--TEST--
Html\render_component: slot-routing validation errors (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;
use Html\Element as E;
use Html\Fragment;
use Html\Slot;

function err(callable $fn): string {
    try { $fn(); return "no error"; }
    catch (\Throwable $e) { return get_class($e) . ': ' . $e->getMessage(); }
}

// Component with one anonymous and one named slot.
function Box(
    #[Slot] Html\Htmlable $body,
    #[Slot('aside')] ?Html\Htmlable $aside = null,
): Html\Htmlable {
    return new E('div', [], [$body, $aside]);
}

// Body content given, but a parameter+attribute would collide.
function Solo(#[Slot] Html\Htmlable $body): Html\Htmlable { return new E('p', [], [$body]); }
echo err(fn() => render_component('Solo', ['body' => 'x'], new Fragment([Html\raw('y')]))), "\n";

// A named slot with no matching #[Html\Slot('name')] parameter.
echo err(fn() => render_component('Box', [], new Fragment([Html\raw('b')]), ['nope' => new Fragment([])])), "\n";

// Body content given to a component that has no anonymous slot.
function NoSlot(string $x): Html\Htmlable { return new E('i', [], [$x]); }
echo err(fn() => render_component('NoSlot', ['x' => 'hi'], new Fragment([Html\raw('body')]))), "\n";

// More than one anonymous slot is a definition error.
function TwoAnon(#[Slot] Html\Htmlable $a, #[Slot] Html\Htmlable $b): Html\Htmlable {
    return new Fragment([$a, $b]);
}
echo err(fn() => render_component('TwoAnon', [], new Fragment([]))), "\n";
?>
--EXPECTF--
Error: Parameter $body is filled by both an attribute and body content
Error: No #[Html\Slot("nope")] parameter accepts the <slot:nope> block
Error: Component received body content but has no anonymous #[Html\Slot] parameter
Error: Component has more than one anonymous #[Html\Slot] parameter
