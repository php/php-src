--TEST--
Markup syntax: dynamic tags <$tag>…</$tag> (RFC §4 variable elements/components)
--EXTENSIONS--
html
--FILE--
<?php
// An element name: behaves exactly like the static tag.
$tag = 'div';
$el = <$tag class="box">Hello</$tag>;
var_dump($el instanceof Html\Element);
echo $el, "\n";

// Self-close; void elements serialize as voids.
$tag = 'br';
echo <$tag/>, "\n";
$tag = 'section';
echo <$tag id="main" hidden>Text {1 + 1}</$tag>, "\n";

// Nested inside static markup, with spread attributes.
$tag = 'em';
$attrs = ['class' => 'x'];
echo <div><$tag {...$attrs}>hi</$tag></div>, "\n";

// A class-component name (::class gives the FQCN): full component dispatch.
class Card implements Html\Htmlable {
    public function __construct(
        private string $title,
        #[Html\Slot] private ?Html\Htmlable $body = null,
    ) {}
    public function toHtml(): Html\Htmlable {
        return <article><h1>{$this->title}</h1>{$this->body}</article>;
    }
}
$component = Card::class;
echo <$component title="Hi"/>, "\n";
echo <$component title="Hi">Body</$component>, "\n";

// A function component: capitalized value, resolved like a direct
// render_component() call (class first, then function).
function greeting(string $name): Html\Htmlable {
    return <p>Hello {$name}</p>;
}
$component = 'Greeting';
echo <$component name="Liam"/>, "\n";

// Interpolated content is still escaped, whatever the tag value.
$tag = 'span';
$evil = '<script>alert(1)</script>';
echo <$tag>{$evil}</$tag>, "\n";

// The brace form takes an arbitrary expression, evaluated once, and closes
// anonymously with </> (or self-closes).
class Registry {
    public function getComponentName(): string { return 'div'; }
}
$object = new Registry;
echo <{$object->getComponentName()} class="x">Hello</>, "\n";
$important = true;
echo <{$important ? 'strong' : 'span'}>note</>, "\n";
echo <{Card::class} title="Braced"/>, "\n";

$calls = 0;
function tag_once(): string { global $calls; $calls++; return 'p'; }
echo <{tag_once()}>once</>, "\n";
var_dump($calls);

// Brace tags nest like any other element.
echo <div><{'em'}>in <b>here</b></></div>, "\n";

// In operator position "<$" keeps its comparison meaning.
$a = 5;
$b = 9;
var_dump($a <$b, $b <$a);

// The runtime target is a public function, same as render_component().
echo Html\render_dynamic('p', ['class' => 'y'], ['direct']), "\n";
echo Html\render_dynamic(Card::class, ['title' => 'Direct']), "\n";
?>
--EXPECT--
bool(true)
<div class="box">Hello</div>
<br>
<section id="main" hidden>Text 2</section>
<div><em class="x">hi</em></div>
<article><h1>Hi</h1></article>
<article><h1>Hi</h1>Body</article>
<p>Hello Liam</p>
<span>&lt;script&gt;alert(1)&lt;/script&gt;</span>
<div class="x">Hello</div>
<strong>note</strong>
<article><h1>Braced</h1></article>
<p>once</p>
int(1)
<div><em>in <b>here</b></em></div>
bool(true)
bool(false)
<p class="y">direct</p>
<article><h1>Direct</h1></article>
