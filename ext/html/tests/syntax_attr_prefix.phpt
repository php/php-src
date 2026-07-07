--TEST--
Markup syntax: ":" / "@" as an optional leading character of attribute names (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
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
echo new Html\Element('i', ['@x' => '1'], []), "\n";

// On a component the names were never valid named-argument labels, so - like
// data-* - they are collected by a variadic.
function Btn(string $label, ...$attrs): Html\Htmlable {
    return <button {...$attrs}>{$label}</button>;
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
