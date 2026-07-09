--TEST--
Html\render_component: resolution errors — a component is a class implementing Htmlable (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;

function err(callable $fn): string {
    try { $fn(); return "no error"; }
    catch (\Throwable $e) { return get_class($e) . ': ' . $e->getMessage(); }
}

// Unknown symbol -> hard error.
echo err(fn() => render_component('NoSuchThing')), "\n";

// A function name is not a component (functions are Future Scope).
function Badge(): Html\Htmlable { return Html\raw('FROM FUNCTION'); }
echo err(fn() => render_component('Badge')), "\n";

// A "date"-named tag can never reach the internal date() function.
echo err(fn() => render_component('date', ['datetime' => 'now'])), "\n";

// A class that does NOT implement Htmlable -> hard error.
class Plain {}
echo err(fn() => render_component('Plain')), "\n";

// "Class::method" resolves to a public static method and is called; the
// result must be an Html\Htmlable.
class Author {
    public static function byline(string $name): Html\Htmlable {
        return Html\raw('BY ' . $name);
    }
    public static function broken(): string {
        return 'plain';
    }
}
echo render_component('Author::byline', ['name' => 'Ada'])->__toString(), "\n";
echo err(fn() => render_component('Author::broken')), "\n";
echo err(fn() => render_component('NoSuchClass::method')), "\n";

// A class implementing Htmlable resolves, regardless of same-named functions.
function Widget(): Html\Htmlable { return Html\raw('FROM FUNCTION'); }
class Widget implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw('FROM CLASS'); }
}
echo render_component('Widget')->__toString(), "\n";
?>
--EXPECT--
Error: "NoSuchThing" is not a component: no such class implementing Html\Htmlable
Error: "Badge" is not a component: no such class implementing Html\Htmlable
Error: "date" is not a component: no such class implementing Html\Htmlable
Error: "Plain" is not a component: no such class implementing Html\Htmlable
BY Ada
Error: Component "Author::broken" did not return an Html\Htmlable
Error: Component class "NoSuchClass" not found
FROM CLASS
