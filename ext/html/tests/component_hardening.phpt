--TEST--
Html components: dispatch guards (visibility, staticness, ctor, void children, hooks) and throwing-constructor safety
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;

function expect_error(callable $f): void {
    try {
        $f();
        echo "NO ERROR\n";
    } catch (Error $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

// --- hook-registration TypeErrors ---

// Registering a non-callable hook is an immediate TypeError.
expect_error(fn() => Html\register_component_factory("no_such_function_xyz"));
expect_error(fn() => Html\register_component_decorator([new stdClass(), "nope"]));

// --- void-element children guard ---

// A void element cannot have children (they would be silently dropped).
expect_error(fn() => (string) new Html\Element("br", [], ["x"]));

// --- int-keyed props ---

// Integer-keyed props are rejected on every dispatch path.
class NoAttrs implements Html\Htmlable {
    public function __construct(public string $a = "") {}
    public function toHtml(): Html\Htmlable { return Html\raw("no-attrs"); }
}
expect_error(fn() => render_component("NoAttrs", ["positional"]));

// --- constructor visibility/absence guards ---

// A non-public constructor cannot be dispatched by the engine.
class PrivateCtor implements Html\Htmlable {
    private function __construct() {}
    public function toHtml(): Html\Htmlable { return Html\raw("private"); }
}
expect_error(fn() => render_component("PrivateCtor"));

// A class with no constructor cannot silently swallow props.
class NoCtor implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw("no-ctor"); }
}
expect_error(fn() => render_component("NoCtor", ["title" => "x"]));
echo render_component("NoCtor"), "\n"; // without props it is fine

// --- static-method visibility guards ---

// Static-method components must be public, static, and non-abstract.
class Methods {
    private static function hidden(): Html\Htmlable { return new Html\Fragment([]); }
    public function nonStatic(): Html\Htmlable { return new Html\Fragment([]); }
}
expect_error(fn() => render_component("Methods::hidden"));
expect_error(fn() => render_component("Methods::nonStatic"));

// --- throwing constructor: no heap corruption / double free ---

// A component whose constructor cannot be satisfied by the engine's own `new`
// (a required, non-prop dependency). The thrown error must leave a clean heap:
// the half-built object is released exactly once, not double-freed.
class NeedsDep implements Html\Htmlable {
    public function __construct(private \stdClass $dep) {}
    public function toHtml(): Html\Htmlable { return Html\raw("never"); }
}

for ($i = 0; $i < 3; $i++) {
    try {
        echo render_component('NeedsDep')->__toString();
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
}

// A constructor that throws from its body, too.
class Boom implements Html\Htmlable {
    public function __construct() { throw new \RuntimeException("boom"); }
    public function toHtml(): Html\Htmlable { return Html\raw("never"); }
}
try {
    echo render_component('Boom')->__toString();
} catch (\Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}

echo "clean\n";
?>
--EXPECTF--
TypeError: Html\register_component_factory(): Argument #1 ($factory) must be a valid callback
TypeError: Html\register_component_decorator(): Argument #1 ($decorator) must be a valid callback
Error: Void element <br> cannot have children
Error: Component props must use string keys
Error: Component class "PrivateCtor" has a non-public constructor
Error: Component class "NoCtor" has no constructor, so it cannot accept attributes or slots
no-ctor
Error: Component method Methods::hidden() must be a public static user-defined method
Error: Component method Methods::nonStatic() must be a public static user-defined method
Too few arguments to function NeedsDep::__construct()%A
Too few arguments to function NeedsDep::__construct()%A
Too few arguments to function NeedsDep::__construct()%A
RuntimeException: boom
clean
