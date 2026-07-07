--TEST--
Html\render_component: a class component whose constructor throws does not corrupt the heap
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;

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
Too few arguments to function NeedsDep::__construct()%A
Too few arguments to function NeedsDep::__construct()%A
Too few arguments to function NeedsDep::__construct()%A
RuntimeException: boom
clean
