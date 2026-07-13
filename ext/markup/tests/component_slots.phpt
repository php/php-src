--TEST--
Html components: #[Markup\Slot] body routing, slot validation errors, Slot attribute reflection
--EXTENSIONS--
markup
--FILE--
<?php
use function Markup\render_component;
use Markup\Element as E;
use Markup\Fragment;
use Markup\Slot;

function err(callable $fn): string {
    try { $fn(); return "no error"; }
    catch (\Throwable $e) { return get_class($e) . ': ' . $e->getMessage(); }
}

// --- body content routes to the #[Markup\Slot] parameter ---

// The body slot routes to the #[Slot] parameter; other content areas are
// ordinary props typed Markup\Html (what named slots used to be).
class Layout implements Markup\Html {
    public function __construct(
        public string $lang,
        #[Slot] public ?Markup\Html $body = null,
        public ?Markup\Html $header = null,
        public ?Markup\Html $footer = null,
    ) {}
    public function toHtml(): Markup\Html {
        return new E('main', ['lang' => $this->lang], [$this->header, $this->body, $this->footer]);
    }
}

$out = render_component(
    Layout::class,
    [
        'lang'   => 'en',
        'header' => new E('h1', [], ['Title']),
        'footer' => new Fragment([new E('small', [], ['(c) 2026'])]),
    ],
    new Fragment([new E('p', [], ['loose body'])]),          // body slot
);
echo $out, "\n";

// Body slot only.
class Panel implements Markup\Html {
    public function __construct(
        public string $kind,
        #[Slot] public ?Markup\Html $content = null,
    ) {}
    public function toHtml(): Markup\Html {
        return new E('section', ['class' => $this->kind], [$this->content]);
    }
}
echo render_component(Panel::class, ['kind' => 'note'], new Fragment([Markup\raw('<b>hi</b>')]))->__toString(), "\n";

// --- slot-routing validation errors ---

// Body content given, but a parameter+attribute would collide.
class Solo implements Markup\Html {
    public function __construct(#[Slot] public Markup\Html $body) {}
    public function toHtml(): Markup\Html { return new E('p', [], [$this->body]); }
}
echo err(fn() => render_component(Solo::class, ['body' => 'x'], new Fragment([Markup\raw('y')]))), "\n";

// Body content given to a component that has no slot parameter.
class NoSlot implements Markup\Html {
    public function __construct(public string $x) {}
    public function toHtml(): Markup\Html { return new E('i', [], [$this->x]); }
}
echo err(fn() => render_component(NoSlot::class, ['x' => 'hi'], new Fragment([Markup\raw('body')]))), "\n";

// More than one slot parameter is a definition error.
class TwoSlots implements Markup\Html {
    public function __construct(#[Slot] public ?Markup\Html $a = null, #[Slot] public ?Markup\Html $b = null) {}
    public function toHtml(): Markup\Html { return new Fragment([$this->a, $this->b]); }
}
echo err(fn() => render_component(TwoSlots::class, [], new Fragment([]))), "\n";

// #[Slot] on a variadic parameter is a definition error.
class Variadic implements Markup\Html {
    private array $parts;
    public function __construct(#[Slot] Markup\Html ...$parts) { $this->parts = $parts; }
    public function toHtml(): Markup\Html { return new Fragment($this->parts); }
}
echo err(fn() => render_component(Variadic::class, [], new Fragment([]))), "\n";

// --- Markup\Slot is a bare parameter-target marker attribute ---

function LayoutFn(
    #[Slot] Markup\Html $body,
    ?Markup\Html $footer = null,
): Markup\Html {
    return new Markup\Fragment([$body, $footer]);
}

$ref = new ReflectionFunction('LayoutFn');
foreach ($ref->getParameters() as $param) {
    foreach ($param->getAttributes(Slot::class) as $attr) {
        $slot = $attr->newInstance();
        printf("%s -> %s\n", $param->getName(), get_class($slot));
    }
}

// Slot takes no arguments and only targets parameters.
$attr = (new ReflectionClass(Slot::class))->getAttributes(Attribute::class)[0]->newInstance();
var_dump($attr->flags === Attribute::TARGET_PARAMETER);
?>
--EXPECTF--
<main lang="en"><h1>Title</h1><p>loose body</p><small>(c) 2026</small></main>
<section class="note"><b>hi</b></section>
Error: Parameter $body is filled by both an attribute and body content
Error: Component received body content but has no #[Markup\Slot] parameter
Error: Component has more than one #[Markup\Slot] parameter
Error: #[Markup\Slot] cannot be applied to a variadic parameter
body -> Markup\Slot
bool(true)
