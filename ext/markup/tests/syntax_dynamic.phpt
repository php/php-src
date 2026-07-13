--TEST--
Markup syntax: dynamic tags <$tag>/<{expr}> - classification, escaping, single evaluation, BC, error cases
--EXTENSIONS--
markup
--FILE--
<?php
// --- <$tag> variable elements ---
// An element name: behaves exactly like the static tag.
$tag = 'div';
$el = <$tag class="box">Hello</$tag>;
var_dump($el instanceof Markup\Element);
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

// --- runtime classification: components ---
// A class-component name (::class gives the FQCN): full component dispatch.
class Card implements Markup\Html {
    public function __construct(
        private string $title,
        #[Markup\Slot] private ?Markup\Html $body = null,
    ) {}
    public function toHtml(): Markup\Html {
        return <article><h1>{$this->title}</h1>{$this->body}</article>;
    }
}
$component = Card::class;
echo <$component title="Hi"/>, "\n";
echo <$component title="Hi">Body</$component>, "\n";

// A "Class::method" value dispatches as a static-method component.
class Author {
    public static function byline(string $name): Markup\Html {
        return <i>By {$name}</i>;
    }
}
$component = 'Author::byline';
echo <$component name="Ada"/>, "\n";

// --- escaping ---
// Interpolated content is still escaped, whatever the tag value.
$tag = 'span';
$evil = '<script>alert(1)</script>';
echo <$tag>{$evil}</$tag>, "\n";

// --- <{expr}> brace form: arbitrary expression, anonymous close ---
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

// --- operator-position "<$" keeps its comparison meaning (BC) ---
$a = 5;
$b = 9;
var_dump($a <$b, $b <$a);

// --- render_dynamic() direct calls ---
// The runtime target is a public function, same as render_component().
echo Markup\render_dynamic('p', ['class' => 'y'], ['direct']), "\n";
echo Markup\render_dynamic(Card::class, ['title' => 'Direct']), "\n";

// --- error cases ---
// Mismatched closing variable is a compile error, like </div> for <span>.
try {
    eval('$x = <$a>hi</$b>;');
} catch (CompileError $e) {
    echo $e->getMessage(), "\n";
}

// A brace tag closes anonymously; a named or variable close is a parse error
// (the expression cannot be restated without re-evaluating it).
foreach (['$x = <{"div"}>a</div>;', '$x = <{"div"}>a</$t>;'] as $code) {
    try {
        eval($code);
    } catch (ParseError $e) {
        echo $e->getMessage(), "\n";
    }
}

// An empty tag value is rejected up front.
try {
    Markup\render_dynamic('');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// A value that is no valid tag name fails at serialization (the existing
// Element guard) - a dynamic name can never break out of the markup.
$tag = 'di v';
try {
    echo (string) <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// A lowercase value is always an element, even if a function of that name
// exists (the same rule that makes static <date> an element).
$tag = 'date';
echo <$tag/>, "\n";

// A capitalized value can never reach an internal function like Date().
$tag = 'Date';
try {
    echo <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// A capitalized value with no symbol behind it.
$tag = 'NoSuchComponent';
try {
    echo <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// A "Class::method" value dispatches as a static-method component; a missing
// class is the same hard error the static tag form gives.
$tag = 'NoSuchRegistry::make';
try {
    echo <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
<div class="box">Hello</div>
<br>
<section id="main" hidden>Text 2</section>
<div><em class="x">hi</em></div>
<article><h1>Hi</h1></article>
<article><h1>Hi</h1>Body</article>
<i>By Ada</i>
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
Mismatched markup closing tag: expected </$a>, found </$b>
syntax error, unexpected markup tag name "div", expecting markup tag end
syntax error, unexpected variable "$t", expecting markup tag end
Markup\render_dynamic(): Argument #1 ($tag) cannot be empty
Invalid tag name "di v"
<date></date>
"Date" is not a component: no such class implementing Markup\Html
"NoSuchComponent" is not a component: no such class implementing Markup\Html
Component class "NoSuchRegistry" not found
