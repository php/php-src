--TEST--
Html: component factory/invoker hooks let userland own instantiation (DI) (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\{register_component_factory, unregister_component_factory,
    register_component_invoker};

// A dependency the props never supply; the "container" injects it.
class Clock { public function now(): string { return "T"; } }

// Class component: a DI dependency ($clock) plus a prop ($label).
class TimeBadge implements Html\Htmlable {
    public function __construct(private Clock $clock, public string $label) {}
    public function toHtml(): Html\Htmlable {
        return new Html\Element('span', [], ["{$this->label}:{$this->clock->now()}"]);
    }
}

// Function and static-method components with a DI dependency.
function Stamp(Clock $clock, string $name): Html\Htmlable {
    return new Html\Element('em', [], ["$name@{$clock->now()}"]);
}
class Author {
    public static function byline(Clock $clock, string $name): Html\Htmlable {
        return new Html\Element('p', [], ["$name/{$clock->now()}"]);
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
$call = fn(callable $fn, array $args)
    => $fn(...autowire(new ReflectionFunction(\Closure::fromCallable($fn)), $args));

// (1) Without hooks, the engine's own `new` runs and fails for the missing Clock.
try { echo <TimeBadge label="x"/>; }
catch (\Error $e) { echo "default:  ", $e->getMessage(), "\n"; }

// $defer is first, so a working result proves the chain skips a null-returning
// factory and falls through to $make.
register_component_factory($defer);
register_component_factory($make);
register_component_invoker($call);

echo "class:    ", <TimeBadge label="A"/>, "\n";
echo "func:     ", <Stamp name="B"/>, "\n";
echo "static:   ", <Author::byline name="C"/>, "\n";

// (2) A factory must return an instance of the requested class. Swap $make for a
// bad factory so the chain becomes [$defer, $bad] and the bad one is reached.
var_dump(unregister_component_factory($make));
$bad = fn(string $class, array $args) => new Clock();
register_component_factory($bad);
try { echo <TimeBadge label="e"/>; }
catch (\Error $e) { echo "badreturn: ", $e->getMessage(), "\n"; }

// (3) Unregistering everything restores the engine default. Unregister identity
// is by callable; a fresh closure does not match.
var_dump(unregister_component_factory(fn($c, $a) => null));
var_dump(unregister_component_factory($defer));
var_dump(unregister_component_factory($bad));
try { echo <TimeBadge label="f"/>; }
catch (\Error $e) { echo "restored:  ", $e->getMessage(), "\n"; }
?>
--EXPECT--
default:  TimeBadge::__construct(): Argument #1 ($clock) not passed
class:    <span>A:T</span>
func:     <em>B@T</em>
static:   <p>C/T</p>
bool(true)
badreturn: Component factory for <TimeBadge> did not return an instance of it or null
bool(false)
bool(true)
bool(true)
restored:  TimeBadge::__construct(): Argument #1 ($clock) not passed
