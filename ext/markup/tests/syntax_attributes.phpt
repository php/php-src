--TEST--
Markup syntax: attributes - literal/{expr}/bare-boolean with coercion, {...spread}, ":"/"@" prefixed names
--EXTENSIONS--
markup
--FILE--
<?php
use Markup\Element as E;

// --- literal, {expr}, bare boolean attributes with coercion ---
$url = "/x?a=1&b=2";
$id = 42;
$on = true;
$off = false;
$missing = null;

// literal value, {expr} value, int, bool true/false, null, bare boolean
echo (<a href={$url} class="btn primary" data-id={$id} disabled={$on} hidden={$off} title={$missing} required>link</a>)->__toString(), "\n";

// attribute value is escaped
echo (<div title="a & b < c">x</div>)->__toString(), "\n";

// self-closing element with attributes
echo (<img src={$url} alt="A & B"/>)->__toString(), "\n";

// --- {...$attrs} spread: elements (array merge) ---
// Element: spread merges into the attribute map; values are still escaped.
$extra = ['data-id' => 7, 'class' => 'from-spread', 'title' => 'T & T'];
echo (<a href="/x" {...$extra}>link</a>)->__toString(), "\n";

// Conflict resolution follows array-merge order (last wins).
echo (<a class="base" {...['class' => 'override']}>x</a>)->__toString(), "\n";
echo (<a {...['class' => 'base']} class="explicit">x</a>)->__toString(), "\n";

// --- {...$attrs} spread: components (named args) ---
// Component: spread becomes named arguments (props).
class Card implements Markup\Html {
    public function __construct(public string $title, public string $tone = 'plain') {}
    public function toHtml(): Markup\Html {
        return new E('div', ['class' => 'card-' . $this->tone], [$this->title]);
    }
}
$props = ['title' => 'Hi & bye', 'tone' => 'loud'];
echo (<Card {...$props}/>)->__toString(), "\n";
echo (<Card title="Explicit" {...['tone' => 'soft']}/>)->__toString(), "\n";

// A component variadic collects unknown (e.g. hyphenated) spread keys.
class Box implements Markup\Html {
    private array $attrs;
    public function __construct(public string $kind, ...$attrs) { $this->attrs = $attrs; }
    public function toHtml(): Markup\Html {
        return new E('div', ['class' => $this->kind, ...$this->attrs], ['box']);
    }
}
echo (<Box kind="b" {...['data-x' => '1', 'role' => 'note']}/>)->__toString(), "\n";

// --- ":" / "@" as an optional leading character of attribute names ---
// Vue/Alpine shorthand attribute names: literal value, {expr} value, bare boolean.
$open = 'open';
echo <div :class="{ active: open }" @click="open = !open">t</div>, "\n";
echo <button @click={$open . '()'} :disabled>go</button>, "\n";

// The prefix composes with the rest of the name charset (Alpine long forms,
// modifiers) - only the *leading* character is new.
echo <input @keydown.escape="close()" :aria-label="label"/>, "\n";

// Prefixed keys also pass the runtime name validation: spread and new Element.
$attrs = ['@click' => 'save()', ':class' => 'cls'];
echo <span {...$attrs}>s</span>, "\n";
echo new Markup\Element('i', ['@x' => '1'], []), "\n";

// On a component the names were never valid named-argument labels, so - like
// data-* - they are collected by a variadic.
class Btn implements Markup\Html {
    private array $attrs;
    public function __construct(public string $label, ...$attrs) { $this->attrs = $attrs; }
    public function toHtml(): Markup\Html {
        return <button {...$this->attrs}>{$this->label}</button>;
    }
}
echo <Btn label="Save" @click="save()" :class="btn"/>, "\n";

// Attribute values are still escaped like any other.
echo <div :class={'a & "b"'}>x</div>, "\n";

// Attribute position only: a prefixed name can never begin a tag. At top
// level "<:" / "<@" do not start markup at all; nested in markup content a
// stray "<" gives the dedicated error.
try {
    eval('$x = <div><:foo/></div>;');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}
try {
    eval('$x = <div><@click/></div>;');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}

// The prefix must be followed by a letter, and closing tags take no attributes.
try {
    eval('$x = <div @>t</div>;');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}
try {
    eval('$x = <div>t</div @x>;');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
<a href="/x?a=1&amp;b=2" class="btn primary" data-id="42" disabled required>link</a>
<div title="a &amp; b &lt; c">x</div>
<img src="/x?a=1&amp;b=2" alt="A &amp; B">
<a href="/x" data-id="7" class="from-spread" title="T &amp; T">link</a>
<a class="override">x</a>
<a class="explicit">x</a>
<div class="card-loud">Hi &amp; bye</div>
<div class="card-soft">Explicit</div>
<div class="b" data-x="1" role="note">box</div>
<div :class="{ active: open }" @click="open = !open">t</div>
<button @click="open()" :disabled>go</button>
<input @keydown.escape="close()" :aria-label="label">
<span @click="save()" :class="cls">s</span>
<i @x="1"></i>
<button @click="save()" :class="btn">Save</button>
<div :class="a &amp; &quot;b&quot;">x</div>
Unescaped "<" in markup text; write {'<'} for a literal less-than sign
Unescaped "<" in markup text; write {'<'} for a literal less-than sign
syntax error, unexpected T_ERROR "@", expecting markup tag name or markup tag end or markup self-close or markup interpolation start
syntax error, unexpected T_ERROR "@", expecting markup tag end
