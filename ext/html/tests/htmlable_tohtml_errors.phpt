--TEST--
Html\Htmlable::toHtml(): resolution cycles and throwing implementations are contained
--EXTENSIONS--
html
--FILE--
<?php
// toHtml() returning $this can never produce a node class: bounded, then Error.
class Selfish implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return $this; }
}
try { echo new Selfish(); }
catch (Error $e) { echo get_class($e), ": ", $e->getMessage(), "\n"; }
try { echo <div>{new Selfish()}</div>; }
catch (Error $e) { echo get_class($e), ": ", $e->getMessage(), "\n"; }

// A two-class cycle is caught by the same bound.
class Ping implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return new Pong(); }
}
class Pong implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return new Ping(); }
}
try { echo new Ping(); }
catch (Error $e) { echo get_class($e), ": ", $e->getMessage(), "\n"; }

// An exception thrown inside toHtml() propagates cleanly through the injected
// __toString (both directly and from child position).
class Boom implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { throw new RuntimeException('boom'); }
}
try { echo new Boom(); }
catch (RuntimeException $e) { echo get_class($e), ": ", $e->getMessage(), "\n"; }
try { echo <div>{new Boom()}</div>; }
catch (RuntimeException $e) { echo get_class($e), ": ", $e->getMessage(), "\n"; }

echo "clean\n";
?>
--EXPECT--
Error: Maximum toHtml() resolution depth of 64 exceeded (Selfish::toHtml() never produced an Html\Element, Html\Fragment, or Html\Raw)
Error: Maximum toHtml() resolution depth of 64 exceeded (Selfish::toHtml() never produced an Html\Element, Html\Fragment, or Html\Raw)
Error: Maximum toHtml() resolution depth of 64 exceeded (Ping::toHtml() never produced an Html\Element, Html\Fragment, or Html\Raw)
RuntimeException: boom
RuntimeException: boom
clean
