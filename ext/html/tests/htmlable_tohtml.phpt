--TEST--
Html\Htmlable::toHtml() is the render contract; __toString comes for free (RFC §3)
--EXTENSIONS--
html
--FILE--
<?php
declare(strict_types=1);

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
