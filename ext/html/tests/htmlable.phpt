--TEST--
Html\Htmlable: toHtml() contract and recursive resolution, injected default __toString vs declared __toString, cycles and throwing implementations
--EXTENSIONS--
html
--FILE--
<?php
declare(strict_types=1);

// --- toHtml() contract ---

// toHtml() returns the markup tree directly: no string cast is needed even
// under strict_types, and the produced Element stays inspectable as an object.
class Card implements Html\Htmlable {
    public function __construct(private string $title) {}
    public function toHtml(): Html\Htmlable {
        return <div class="card">{$this->title}</div>;
    }
}

$card = new Card('Hi & bye');
var_dump($card->toHtml() instanceof Html\Element);
var_dump($card->toHtml()->tag);

// The injected default __toString renders via toHtml(), so echo, (string),
// and interpolation all work without the class declaring __toString.
echo $card, "\n";
echo (string) $card, "\n";
echo "wrapped: {$card}\n";
var_dump($card instanceof Stringable);

// In markup child position the component renders through toHtml() natively.
echo <section>{$card}</section>, "\n";

// And as a component tag.
echo <Card title="Tagged"/>, "\n";

// toHtml() may return another Htmlable (e.g. delegate to another component);
// resolution recurses until it reaches a node class.
class Fancy implements Html\Htmlable {
    public function toHtml(): Html\Htmlable {
        return new Card('delegated');
    }
}
echo new Fancy(), "\n";

// The node classes are the base cases: their toHtml() returns themselves.
$el = <b>x</b>;
var_dump($el->toHtml() === $el);
$frag = <>y</>;
var_dump($frag->toHtml() === $frag);
$raw = Html\raw('<i>z</i>');
var_dump($raw->toHtml() === $raw);

// --- injected default __toString vs declared __toString ---

// A __toString declared by the class wins for string casts; markup rendering
// always goes through toHtml(), so the two can serve different audiences
// (e.g. a log-friendly string form) without affecting markup output.
class Custom implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return <b>markup</b>; }
    public function __toString(): string { return 'CUSTOM'; }
}
echo new Custom(), "\n";
echo <i>{new Custom()}</i>, "\n";

// A __toString flattened in from a trait counts as declared.
trait Loud {
    public function __toString(): string { return 'LOUD'; }
}
class UsesTrait implements Html\Htmlable {
    use Loud;
    public function toHtml(): Html\Htmlable { return Html\raw('quiet'); }
}
echo new UsesTrait(), " / ", <u>{new UsesTrait()}</u>, "\n";

// A __toString inherited from a parent class also wins.
class StringyBase {
    public function __toString(): string { return 'BASE'; }
}
class ChildOfStringy extends StringyBase implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw('child'); }
}
echo new ChildOfStringy(), " / ", <u>{new ChildOfStringy()}</u>, "\n";

// An abstract class implementing Htmlable: concrete children inherit the
// injected default.
abstract class Widget implements Html\Htmlable {}
class Button extends Widget {
    public function toHtml(): Html\Htmlable { return <button>ok</button>; }
}
echo new Button(), "\n";

// An interface extending Htmlable stays abstract; implementers get the default.
interface Panel extends Html\Htmlable {}
class SidePanel implements Panel {
    public function toHtml(): Html\Htmlable { return <aside>side</aside>; }
}
echo new SidePanel(), "\n";

// The injected method is a real internal method: visible to reflection, with
// the declared string return type, owned by the implementing class.
$rm = new ReflectionMethod(SidePanel::class, '__toString');
var_dump($rm->isInternal(), (string) $rm->getReturnType(), $rm->getDeclaringClass()->getName());
var_dump(method_exists(new Button(), '__toString'));
var_dump(is_callable([new Button(), '__toString']));
echo (new Button())->__toString(), "\n";

// --- resolution cycles and throwing implementations ---

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
bool(true)
string(3) "div"
<div class="card">Hi &amp; bye</div>
<div class="card">Hi &amp; bye</div>
wrapped: <div class="card">Hi &amp; bye</div>
bool(true)
<section><div class="card">Hi &amp; bye</div></section>
<div class="card">Tagged</div>
<div class="card">delegated</div>
bool(true)
bool(true)
bool(true)
CUSTOM
<i><b>markup</b></i>
LOUD / <u>quiet</u>
BASE / <u>child</u>
<button>ok</button>
<aside>side</aside>
bool(true)
string(6) "string"
string(9) "SidePanel"
bool(true)
bool(true)
<button>ok</button>
Error: Maximum toHtml() resolution depth of 64 exceeded (Selfish::toHtml() never produced an Html\Element, Html\Fragment, or Html\Raw)
Error: Maximum toHtml() resolution depth of 64 exceeded (Selfish::toHtml() never produced an Html\Element, Html\Fragment, or Html\Raw)
Error: Maximum toHtml() resolution depth of 64 exceeded (Ping::toHtml() never produced an Html\Element, Html\Fragment, or Html\Raw)
RuntimeException: boom
RuntimeException: boom
clean
