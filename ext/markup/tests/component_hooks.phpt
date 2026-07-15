--TEST--
Html components: factory hooks (DI), decorators, and per-component hook scoping
--EXTENSIONS--
markup
--FILE--
<?php
use function Markup\{register_component_factory, unregister_component_factory,
    register_component_decorator, unregister_component_decorator};

// Shared components across the sections below.
class Card implements Markup\Html {
    public function __construct(public string $title = "?") {}
    public function toHtml(): Markup\Html {
        return new Markup\Element('div', [], [$this->title]);
    }
}
class Badge implements Markup\Html {
    public function __construct(public string $label = "?") {}
    public function toHtml(): Markup\Html {
        return new Markup\Element('b', [], [$this->label]);
    }
}
class Note implements Markup\Html {
    public function __construct(public string $t) {}
    public function toHtml(): Markup\Html {
        return new Markup\Element('p', [], [$this->t]);
    }
}
class Author {
    public static function byline(string $t): Markup\Html {
        return new Markup\Element('span', [], [$t]);
    }
}

// --- factory hooks: chain, defer, bad return, unregister identity ---

// A dependency the props never supply; the "container" injects it.
class Clock { public function now(): string { return "T"; } }

// Class component: a DI dependency ($clock) plus a prop ($label).
class TimeBadge implements Markup\Html {
    public function __construct(private Clock $clock, public string $label) {}
    public function toHtml(): Markup\Html {
        return new Markup\Element('span', [], ["{$this->label}:{$this->clock->now()}"]);
    }
}

// A tiny autowiring container: fill non-builtin params the $args don't provide.
function autowire(ReflectionFunctionAbstract $r, array $args): array {
    $extra = [];
    foreach ($r->getParameters() as $p) {
        $t = $p->getType();
        if ($t instanceof ReflectionNamedType && !$t->isBuiltin()
                && !array_key_exists($p->getName(), $args)) {
            $extra[$p->getName()] = new ($t->getName())();
        }
    }
    return [...$extra, ...$args];
}
$defer = fn(string $class, array $args) => null; // always defers to the next factory
$make = fn(string $class, array $args)
    => (new ReflectionClass($class))->newInstanceArgs(
        autowire(new ReflectionMethod($class, '__construct'), $args));

// (1) Without hooks, the engine's own `new` runs and fails for the missing Clock.
try { echo <TimeBadge label="x"/>; }
catch (\Error $e) { echo "default:  ", $e->getMessage(), PHP_EOL; }

// $defer is first, so a working result proves the chain skips a null-returning
// factory and falls through to $make.
register_component_factory($defer);
register_component_factory($make);

echo "class:    ", <TimeBadge label="A"/>, PHP_EOL;

// (2) A factory must return an instance of the requested class. Swap $make for a
// bad factory so the chain becomes [$defer, $bad] and the bad one is reached.
var_dump(unregister_component_factory($make));
$bad = fn(string $class, array $args) => new Clock();
register_component_factory($bad);
try { echo <TimeBadge label="e"/>; }
catch (\Error $e) { echo "badreturn: ", $e->getMessage(), PHP_EOL; }

// (3) Unregistering everything restores the engine default. Unregister identity
// is by callable; a fresh closure does not match.
var_dump(unregister_component_factory(fn($c, $a) => null));
var_dump(unregister_component_factory($defer));
var_dump(unregister_component_factory($bad));
try { echo <TimeBadge label="f"/>; }
catch (\Error $e) { echo "restored:  ", $e->getMessage(), PHP_EOL; }

// --- decorators: uniform wrapping, composition, bad return ---

// A decorator wrapping in a marker that records the component name; proves it
// fires for class and static-method components alike, with the resolved name.
$marker = fn(Markup\Html $h, string $c) => Markup\raw("[$c]" . $h . "[/$c]");
register_component_decorator($marker);

echo <Card title="a"/>, PHP_EOL;
echo <Note t="b"/>, PHP_EOL;
echo <Author::byline t="c"/>, PHP_EOL;

// Decorators compose in registration order, each wrapping the previous result.
$outer = fn(Markup\Html $h, string $c) => Markup\raw("<<" . $h . ">>");
register_component_decorator($outer);
echo <Note t="d"/>, PHP_EOL;

// A decorator must return a Markup\Html.
$badDecorator = fn(Markup\Html $h, string $c) => "not html";
register_component_decorator($badDecorator);
try { echo <Note t="e"/>; }
catch (\Error $e) { echo $e->getMessage(), PHP_EOL; }

// Clean up this section's decorators so the next section starts from default.
unregister_component_decorator($badDecorator);
unregister_component_decorator($outer);
unregister_component_decorator($marker);

// --- per-component scoping of factories and decorators ---

// (1) A factory scoped to Card fires for <Card/> only; <Badge/> keeps the
// engine default. Scope names are case-insensitive and a leading backslash
// is accepted.
$cardFactory = function (string $class, array $args) {
    echo "factory($class)\n";
    return new Card("made");
};
register_component_factory($cardFactory, '\cArD');
echo <Card/>, PHP_EOL;
echo <Badge/>, PHP_EOL;

// (2) Scoped and unscoped factories share one chain in registration order:
// the scoped one (registered first) wins for Card; the unscoped one sees Badge.
$anyFactory = function (string $class, array $args) {
    echo "any($class)\n";
    return null; // defer
};
register_component_factory($anyFactory);
echo <Card/>, PHP_EOL;
echo <Badge/>, PHP_EOL;

// (3) Unregister identity includes the scope: the callable alone (or with the
// wrong scope) does not match; with its scope it does.
var_dump(unregister_component_factory($cardFactory));
var_dump(unregister_component_factory($cardFactory, 'Badge'));
var_dump(unregister_component_factory($cardFactory, 'Card'));
var_dump(unregister_component_factory($anyFactory));
echo <Card/>, PHP_EOL;

// (4) A decorator scoped to Badge wraps only Badge's output - Card renders
// untouched.
$badgeDecorator = fn(Markup\Html $h, string $c) => Markup\raw("<<$h>>");
register_component_decorator($badgeDecorator, Badge::class);
echo <Badge label="B"/>, PHP_EOL;
echo <Card title="C"/>, PHP_EOL;
var_dump(unregister_component_decorator($badgeDecorator, 'badge'));
echo <Badge label="B"/>, PHP_EOL;
?>
--EXPECT--
default:  TimeBadge::__construct(): Argument #1 ($clock) not passed
class:    <span>A:T</span>
bool(true)
badreturn: Component factory for <TimeBadge> did not return an instance of it or null
bool(false)
bool(true)
bool(true)
restored:  TimeBadge::__construct(): Argument #1 ($clock) not passed
[Card]<div>a</div>[/Card]
[Note]<p>b</p>[/Note]
[Author::byline]<span>c</span>[/Author::byline]
<<[Note]<p>d</p>[/Note]>>
Component decorator for <Note> did not return a Markup\Html
factory(Card)
<div>made</div>
<b>?</b>
factory(Card)
<div>made</div>
any(Badge)
<b>?</b>
bool(false)
bool(false)
bool(true)
bool(true)
<div>?</div>
<<<b>B</b>>>
<div>C</div>
bool(true)
<b>B</b>
