--TEST--
Html: dispatch guards - visibility, staticness, missing constructor, void children, hook validation
--EXTENSIONS--
html
--FILE--
<?php

function expect_error(callable $f): void {
    try {
        $f();
        echo "NO ERROR\n";
    } catch (Error $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

// 1. Registering a non-callable hook is an immediate TypeError.
expect_error(fn() => Html\register_component_factory("no_such_function_xyz"));
expect_error(fn() => Html\register_component_invoker(42));
expect_error(fn() => Html\register_component_decorator([new stdClass(), "nope"]));

// 2. A void element cannot have children (they would be silently dropped).
expect_error(fn() => (string) new Html\Element("br", [], ["x"]));

// 3. Integer-keyed props are rejected on every dispatch path.
class NoAttrs implements Html\Htmlable {
    public function __construct(public string $a = "") {}
    public function toHtml(): Html\Htmlable { return Html\raw("no-attrs"); }
}
expect_error(fn() => Html\render_component("NoAttrs", ["positional"]));

// 4. A non-public constructor cannot be dispatched by the engine.
class PrivateCtor implements Html\Htmlable {
    private function __construct() {}
    public function toHtml(): Html\Htmlable { return Html\raw("private"); }
}
expect_error(fn() => Html\render_component("PrivateCtor"));

// 5. A class with no constructor cannot silently swallow props.
class NoCtor implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw("no-ctor"); }
}
expect_error(fn() => Html\render_component("NoCtor", ["title" => "x"]));
echo Html\render_component("NoCtor"), "\n"; // without props it is fine

// 6. Static-method components must be public, static, and non-abstract.
class Methods {
    private static function hidden(): Html\Htmlable { return new Html\Fragment([]); }
    public function nonStatic(): Html\Htmlable { return new Html\Fragment([]); }
}
expect_error(fn() => Html\render_component("Methods::hidden"));
expect_error(fn() => Html\render_component("Methods::nonStatic"));

?>
--EXPECTF--
TypeError: Html\register_component_factory(): Argument #1 ($factory) must be a valid callback
TypeError: Html\register_component_invoker(): Argument #1 ($invoker) must be a valid callback
TypeError: Html\register_component_decorator(): Argument #1 ($decorator) must be a valid callback
Error: Void element <br> cannot have children
Error: Component props must use string keys
Error: Component class "PrivateCtor" has a non-public constructor
Error: Component class "NoCtor" has no constructor, so it cannot accept attributes or slots
no-ctor
Error: Component method Methods::hidden() must be a public static user-defined method
Error: Component method Methods::nonStatic() must be a public static user-defined method
