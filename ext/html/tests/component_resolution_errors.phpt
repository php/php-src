--TEST--
Html\render_component: bare-tag resolution order and the date() footgun guard (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;

function err(callable $fn): string {
    try { $fn(); return "no error"; }
    catch (\Throwable $e) { return get_class($e) . ': ' . $e->getMessage(); }
}

// Resolves to the internal date() -> hard error, never silently called.
echo err(fn() => render_component('date', ['datetime' => 'now'])), "\n";

// Unknown symbol -> hard error.
echo err(fn() => render_component('NoSuchThing')), "\n";

// A userland function that does NOT return Htmlable -> hard error.
function NotHtml(): string { return 'plain'; }
echo err(fn() => render_component('NotHtml')), "\n";

// A class implementing Htmlable wins over a same-named function.
function Widget(): Html\Htmlable { return Html\raw('FROM FUNCTION'); }
class Widget implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw('FROM CLASS'); }
}
echo render_component('Widget')->__toString(), "\n";
?>
--EXPECTF--
Error: <date> resolved to the internal function date(), which cannot be a component
Error: "NoSuchThing" is not a component: no class implementing Html\Htmlable and no user-defined function of that name
Error: Component "NotHtml" did not return an Html\Htmlable
FROM CLASS
