--TEST--
Html\render_component: slot-routing validation errors (RFC §5)
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

// Body content given, but a parameter+attribute would collide.
function Solo(#[Slot] Html\Htmlable $body): Html\Htmlable { return new E('p', [], [$body]); }
echo err(fn() => render_component('Solo', ['body' => 'x'], new Fragment([Html\raw('y')]))), "\n";

// Body content given to a component that has no slot parameter.
function NoSlot(string $x): Html\Htmlable { return new E('i', [], [$x]); }
echo err(fn() => render_component('NoSlot', ['x' => 'hi'], new Fragment([Html\raw('body')]))), "\n";

// More than one slot parameter is a definition error.
function TwoSlots(#[Slot] Html\Htmlable $a, #[Slot] Html\Htmlable $b): Html\Htmlable {
    return new Fragment([$a, $b]);
}
echo err(fn() => render_component('TwoSlots', [], new Fragment([]))), "\n";

// #[Slot] on a variadic parameter is a definition error.
function Variadic(#[Slot] Html\Htmlable ...$parts): Html\Htmlable { return new Fragment($parts); }
echo err(fn() => render_component('Variadic', [], new Fragment([]))), "\n";
?>
--EXPECTF--
Error: Parameter $body is filled by both an attribute and body content
Error: Component received body content but has no #[Html\Slot] parameter
Error: Component has more than one #[Html\Slot] parameter
Error: #[Html\Slot] cannot be applied to a variadic parameter
