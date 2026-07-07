--TEST--
Html: hooks scoped to one component via the optional $component argument (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\{register_component_factory, unregister_component_factory,
    register_component_invoker, unregister_component_invoker,
    register_component_decorator, unregister_component_decorator};

class Card implements Html\Htmlable {
    public function __construct(public string $title = "?") {}
    public function toHtml(): Html\Htmlable {
        return new Html\Element('div', [], [$this->title]);
    }
}
class Badge implements Html\Htmlable {
    public function __construct(public string $label = "?") {}
    public function toHtml(): Html\Htmlable {
        return new Html\Element('b', [], [$this->label]);
    }
}
function Stamp(string $name = "?"): Html\Htmlable {
    return new Html\Element('em', [], [$name]);
}
class Author {
    public static function byline(string $name = "?"): Html\Htmlable {
        return new Html\Element('p', [], [$name]);
    }
}

// (1) A factory scoped to Card fires for <Card/> only; <Badge/> keeps the
// engine default. Scope names are case-insensitive and a leading backslash
// is accepted.
$cardFactory = function (string $class, array $args) {
    echo "factory($class)\n";
    return new Card("made");
};
register_component_factory($cardFactory, '\cArD');
echo <Card/>, "\n";
echo <Badge/>, "\n";

// (2) Scoped and unscoped factories share one chain in registration order:
// the scoped one (registered first) wins for Card; the unscoped one sees Badge.
$anyFactory = function (string $class, array $args) {
    echo "any($class)\n";
    return null; // defer
};
register_component_factory($anyFactory);
echo <Card/>, "\n";
echo <Badge/>, "\n";

// (3) Unregister identity includes the scope: the callable alone (or with the
// wrong scope) does not match; with its scope it does.
var_dump(unregister_component_factory($cardFactory));
var_dump(unregister_component_factory($cardFactory, 'Badge'));
var_dump(unregister_component_factory($cardFactory, 'Card'));
var_dump(unregister_component_factory($anyFactory));
echo <Card/>, "\n";

// (4) Invokers scope on the resolved callable name: the function's name for
// <Stamp/>, "Class::method" for <Author::byline/>.
$stampInvoker = function (callable $fn, array $args) {
    echo "invoke(", strtolower(is_string($fn) ? $fn : "?"), ")\n";
    return new Html\Raw("[stamped]");
};
register_component_invoker($stampInvoker, 'STAMP');
echo <Stamp name="a"/>, "\n";
echo <Author::byline name="b"/>, "\n";
var_dump(unregister_component_invoker($stampInvoker, 'stamp'));

$bylineInvoker = fn(callable $fn, array $args) => new Html\Raw("[bylined]");
register_component_invoker($bylineInvoker, 'author::BYLINE');
echo <Stamp name="c"/>, "\n";
echo <Author::byline name="d"/>, "\n";
var_dump(unregister_component_invoker($bylineInvoker, '\Author::byline'));

// (5) A decorator scoped to Badge wraps only Badge's output — Card, Stamp and
// the rest render untouched.
$badgeDecorator = fn(Html\Htmlable $h, string $c) => Html\raw("<<$h>>");
register_component_decorator($badgeDecorator, Badge::class);
echo <Badge label="B"/>, "\n";
echo <Card title="C"/>, "\n";
echo <Stamp name="s"/>, "\n";
var_dump(unregister_component_decorator($badgeDecorator, 'badge'));
echo <Badge label="B"/>, "\n";
?>
--EXPECT--
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
invoke(stamp)
[stamped]
<p>b</p>
bool(true)
<em>c</em>
[bylined]
bool(true)
<<<b>B</b>>>
<div>C</div>
<em>s</em>
bool(true)
<b>B</b>
