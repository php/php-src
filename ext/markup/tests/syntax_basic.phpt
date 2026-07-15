--TEST--
Markup syntax: elements, fragments, interpolation, whitespace, childless/self-close, literal braces, comments
--EXTENSIONS--
markup
--FILE--
<?php
// --- elements, interpolation, nesting, fragments ---
$name = "Ada & <friends>";

// Element with interpolation and a nested element.
$el = <div>Hello {$name} <em>world</em></div>;
var_dump($el instanceof Markup\Element);
echo $el->__toString(), PHP_EOL;

// Fragment (no wrapper).
$frag = <><h1>Title</h1><p>Body {$name}</p></>;
var_dump($frag instanceof Markup\Fragment);
echo $frag->__toString(), PHP_EOL;

// Interpolation can hold any expression; arrays flatten, null renders "".
$items = ['x', 'y'];
echo (<ul>{array_map(fn($i) => <li>{$i}</li>, $items)}</ul>)->__toString(), PHP_EOL;
echo (<p>{true ? 'yes' : null}</p>)->__toString(), PHP_EOL;

// --- JSX-style whitespace normalization ---
// Newlines + indentation between block elements are dropped.
echo (<ul>
    <li>one</li>
    <li>two</li>
</ul>)->__toString(), PHP_EOL;

// A meaningful single space between inline content is preserved.
$who = 'Ada';
echo (<p>Hello {$who}, <em>welcome</em> back</p>)->__toString(), PHP_EOL;

// Leading/trailing blank lines and indentation around text collapse away.
echo (<p>
    Just some text.
</p>)->__toString(), PHP_EOL;

// Words on separate lines join with a single space.
echo (<p>
    one
    two
    three
</p>)->__toString(), PHP_EOL;

// Inline trailing space with no newline is kept (so adjacent elements don't fuse).
echo (<p>a <b>b</b> c</p>)->__toString(), PHP_EOL;

// --- childless elements: empty or self-closed are equivalent ---
// <div></div> and <div/> are equivalent - both allowed, so real HTML can be
// pasted into markup unchanged.
var_dump((string) <div></div> === (string) <div/>);
echo <div></div>, PHP_EOL;
echo <span class="a"></span>, PHP_EOL;

// A whitespace-only multi-line body normalizes away to the same empty element.
echo <div>
</div>, PHP_EOL;

// Void elements still serialize with no closing tag, from either source form.
echo <br/>, PHP_EOL;
echo <br></br>, PHP_EOL;

// --- self-closing source; serializer expands non-void elements ---
// Void elements serialize without a closing tag...
echo (<img/>)->__toString(), PHP_EOL;
echo (<img src="/a.png"/>)->__toString(), PHP_EOL;
// ...non-void self-closed source expands to open+close output.
echo (<div/>)->__toString(), PHP_EOL;
echo (<span class="x"/>)->__toString(), PHP_EOL;

// Empty components and fragments are allowed (only HTML elements must self-close).
class C implements Markup\Html {
    public function toHtml(): Markup\Html { return Markup\raw('C'); }
}
echo (<C></C>)->__toString(), PHP_EOL;
var_dump((<></>)->__toString());

// --- literal braces via character references or {'{'} ---
// { begins interpolation; a literal { is written as in HTML or JSX.

// Character references decode to literal braces at compile time (no interpolation).
echo (<p>&#123;x&#125;</p>)->__toString(), PHP_EOL;
echo (<p>&lbrace;x&rbrace;</p>)->__toString(), PHP_EOL;

// The JSX form: an interpolated string literal.
echo (<p>{'{'}x{'}'}</p>)->__toString(), PHP_EOL;

// A lone } in text is already literal.
echo (<p>a } b</p>)->__toString(), PHP_EOL;

// Literal JSON in a <script> uses the raw() island pattern (text children are
// HTML-escaped, and browsers never entity-decode inside raw-text elements).
echo (<script type="application/json" id="config">{Markup\raw(<<<'JSON'
{"@type": "Article", "title": "Braces & quotes stay verbatim"}
JSON)}</script>)->__toString(), PHP_EOL;

// --- comments: <!-- --> is literal output; {/* */} renders nothing ---
$x = 'ignored';

// <!-- --> is emitted verbatim; its content is literal (no interpolation).
echo (<div><!-- a literal {$x} comment --><p>body</p></div>)->__toString(), PHP_EOL;

// {/* ... */} is a source-only comment and renders nothing.
echo (<p>a{/* dropped */}b</p>)->__toString(), PHP_EOL;

// an empty interpolation renders nothing too
echo (<p>x{}y</p>)->__toString(), PHP_EOL;

// comment between block elements survives (it is real output, not whitespace)
echo (<ul>
    <!-- first -->
    <li>one</li>
</ul>)->__toString(), PHP_EOL;

// --- operand positions: markup lexes after every operand-expecting token ---
// (the scanner's allowlist; a comparison never silently changes meaning)
echo "concat " . <u>c</u>, PHP_EOL;                      // after "."
echo true ? <s>t</s> : <s>f</s>, PHP_EOL;                // after "?" and ":"
echo null ?? <em>coal</em>, PHP_EOL;                     // after "??"
echo (string) <p>cast</p>, PHP_EOL;                      // after a cast
echo <div>pipe</div> |> strtoupper(...), PHP_EOL;        // after "|>"
echo clone <span>cl</span>, PHP_EOL;                     // after `clone`
print <p>print</p>; echo PHP_EOL;                        // after `print`
echo [<b>a</b>, <b>b</b>][1], PHP_EOL;                   // after "[" and ","
echo (fn() => <li>arrow</li>)(), PHP_EOL;                // after "=>"
echo match(true) { true => <b>match</b> }, PHP_EOL;      // match arm "=>"
function gen() { yield from [<i>yf</i>]; }            // after `yield from`
echo iterator_to_array(gen())[0], PHP_EOL;
?>
--EXPECT--
bool(true)
<div>Hello Ada &amp; &lt;friends&gt; <em>world</em></div>
bool(true)
<h1>Title</h1><p>Body Ada &amp; &lt;friends&gt;</p>
<ul><li>x</li><li>y</li></ul>
<p>yes</p>
<ul><li>one</li><li>two</li></ul>
<p>Hello Ada, <em>welcome</em> back</p>
<p>Just some text.</p>
<p>one two three</p>
<p>a <b>b</b> c</p>
bool(true)
<div></div>
<span class="a"></span>
<div></div>
<br>
<br>
<img>
<img src="/a.png">
<div></div>
<span class="x"></span>
C
string(0) ""
<p>{x}</p>
<p>{x}</p>
<p>{x}</p>
<p>a } b</p>
<script type="application/json" id="config">{"@type": "Article", "title": "Braces & quotes stay verbatim"}</script>
<div><!-- a literal {$x} comment --><p>body</p></div>
<p>ab</p>
<p>xy</p>
<ul><!-- first --><li>one</li></ul>
concat <u>c</u>
<s>t</s>
<em>coal</em>
<p>cast</p>
<DIV>PIPE</DIV>
<span>cl</span>
<p>print</p>
<b>b</b>
<li>arrow</li>
<b>match</b>
<i>yf</i>
