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
class Solo implements Html\Htmlable {
    public function __construct(#[Slot] public Html\Htmlable $body) {}
    public function toHtml(): Html\Htmlable { return new E('p', [], [$this->body]); }
}
echo err(fn() => render_component(Solo::class, ['body' => 'x'], new Fragment([Html\raw('y')]))), "\n";

// Body content given to a component that has no slot parameter.
class NoSlot implements Html\Htmlable {
    public function __construct(public string $x) {}
    public function toHtml(): Html\Htmlable { return new E('i', [], [$this->x]); }
}
echo err(fn() => render_component(NoSlot::class, ['x' => 'hi'], new Fragment([Html\raw('body')]))), "\n";

// More than one slot parameter is a definition error.
class TwoSlots implements Html\Htmlable {
    public function __construct(#[Slot] public ?Html\Htmlable $a = null, #[Slot] public ?Html\Htmlable $b = null) {}
    public function toHtml(): Html\Htmlable { return new Fragment([$this->a, $this->b]); }
}
echo err(fn() => render_component(TwoSlots::class, [], new Fragment([]))), "\n";

// #[Slot] on a variadic parameter is a definition error.
class Variadic implements Html\Htmlable {
    private array $parts;
    public function __construct(#[Slot] Html\Htmlable ...$parts) { $this->parts = $parts; }
    public function toHtml(): Html\Htmlable { return new Fragment($this->parts); }
}
echo err(fn() => render_component(Variadic::class, [], new Fragment([]))), "\n";
?>
--EXPECTF--
Error: Parameter $body is filled by both an attribute and body content
Error: Component received body content but has no #[Html\Slot] parameter
Error: Component has more than one #[Html\Slot] parameter
Error: #[Html\Slot] cannot be applied to a variadic parameter
