--TEST--
Html components: tag lowering, class/static-method dispatch, resolution errors, hyphenated attrs (RFC §4, §5)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;
use Html\Slot;
use function Html\render_component;

function err(callable $fn): string {
    try { $fn(); return "no error"; }
    catch (\Throwable $e) { return get_class($e) . ': ' . $e->getMessage(); }
}

// --- component tag lowering: class + static-method dispatch ---

// Class component implementing Htmlable, with a prop and an anonymous slot.
class Card implements Html\Htmlable {
    public function __construct(
        public string $title,
        #[Slot] public ?Html\Htmlable $body = null,
    ) {}
    public function toHtml(): Html\Htmlable {
        return new E('section', ['class' => 'card'], [new E('h2', [], [$this->title]), $this->body]);
    }
}

// A second, minimal component for nesting cases.
class Badge implements Html\Htmlable {
    public function __construct(public string $label) {}
    public function toHtml(): Html\Htmlable {
        return new E('span', ['class' => 'badge'], [$this->label]);
    }
}

// Static-method component - multiple components can live together on one class.
// (broken() returns a non-Htmlable, for the error section below.)
class Author {
    public static function byline(string $name): Html\Htmlable {
        return new E('p', ['class' => 'by'], ['By ', $name]);
    }
    public static function broken(): string {
        return 'plain';
    }
}

$who = 'Ada & co';

// class component: attribute prop + body routed to the anonymous slot
echo (<Card title={$who}><p>Hi {$who}</p></Card>)->__toString(), "\n";

// self-closing class component (no body -> slot is null)
echo (<Card title="Solo"/>)->__toString(), "\n";

// static-method component
echo (<Author::byline name={$who}/>)->__toString(), "\n";

// the same dispatch, called directly by name
echo render_component(Badge::class, ['label' => 'New &'])->__toString(), "\n";

// a component used as a child of an HTML element, and inside interpolation
echo (<div class="wrap"><Badge label="x"/></div>)->__toString(), "\n";
echo (<ul>{array_map(fn($t) => <Badge label={$t}/>, ['a', 'b'])}</ul>)->__toString(), "\n";

// --- resolution errors: a component is a class implementing Htmlable ---

// Unknown symbol -> hard error.
echo err(fn() => render_component('NoSuchThing')), "\n";

// A function name is not a component (functions are Future Scope).
function Chip(): Html\Htmlable { return Html\raw('FROM FUNCTION'); }
echo err(fn() => render_component('Chip')), "\n";

// A "date"-named tag can never reach the internal date() function.
echo err(fn() => render_component('date', ['datetime' => 'now'])), "\n";

// A class that does NOT implement Htmlable -> hard error.
class Plain {}
echo err(fn() => render_component('Plain')), "\n";

// "Class::method" resolves to a public static method and is called; the
// result must be an Html\Htmlable.
echo render_component('Author::byline', ['name' => 'Ada'])->__toString(), "\n";
echo err(fn() => render_component('Author::broken')), "\n";
echo err(fn() => render_component('NoSuchClass::method')), "\n";

// A class implementing Htmlable resolves, regardless of same-named functions.
function Widget(): Html\Htmlable { return Html\raw('FROM FUNCTION'); }
class Widget implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw('FROM CLASS'); }
}
echo render_component('Widget')->__toString(), "\n";

// --- hyphenated attributes route through named args ---

// A component that collects arbitrary (incl. hyphenated) attributes via a variadic.
class Box implements Html\Htmlable {
    private array $attrs;
    public function __construct(public string $kind, ...$attrs) { $this->attrs = $attrs; }
    public function toHtml(): Html\Htmlable {
        return new E('div', ['class' => $this->kind, ...$this->attrs], ['box']);
    }
}

// Hyphenated attributes work directly on a component (no spread needed) - they
// become named arguments and the variadic collects them.
echo (<Box kind="note" data-id="7" aria-label="hi"/>), "\n";

// A component without a variadic rejects an unknown (hyphenated) attribute.
class Tight implements Html\Htmlable {
    public function __construct(public string $kind) {}
    public function toHtml(): Html\Htmlable { return new E('div', ['class' => $this->kind], ['y']); }
}
echo err(fn() => (<Tight kind="b" data-x="1"/>)->__toString()), "\n";
?>
--EXPECT--
<section class="card"><h2>Ada &amp; co</h2><p>Hi Ada &amp; co</p></section>
<section class="card"><h2>Solo</h2></section>
<p class="by">By Ada &amp; co</p>
<span class="badge">New &amp;</span>
<div class="wrap"><span class="badge">x</span></div>
<ul><span class="badge">a</span><span class="badge">b</span></ul>
Error: "NoSuchThing" is not a component: no such class implementing Html\Htmlable
Error: "Chip" is not a component: no such class implementing Html\Htmlable
Error: "date" is not a component: no such class implementing Html\Htmlable
Error: "Plain" is not a component: no such class implementing Html\Htmlable
<p class="by">By Ada</p>
Error: Component "Author::broken" did not return an Html\Htmlable
Error: Component class "NoSuchClass" not found
FROM CLASS
<div class="note" data-id="7" aria-label="hi">box</div>
Error: Unknown named parameter $data-x
