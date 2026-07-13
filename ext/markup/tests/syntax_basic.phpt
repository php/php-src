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
echo $el->__toString(), "\n";

// Fragment (no wrapper).
$frag = <><h1>Title</h1><p>Body {$name}</p></>;
var_dump($frag instanceof Markup\Fragment);
echo $frag->__toString(), "\n";

// Interpolation can hold any expression; arrays flatten, null renders "".
$items = ['x', 'y'];
echo (<ul>{array_map(fn($i) => <li>{$i}</li>, $items)}</ul>)->__toString(), "\n";
echo (<p>{true ? 'yes' : null}</p>)->__toString(), "\n";

// --- JSX-style whitespace normalization ---
// Newlines + indentation between block elements are dropped.
echo (<ul>
    <li>one</li>
    <li>two</li>
</ul>)->__toString(), "\n";

// A meaningful single space between inline content is preserved.
$who = 'Ada';
echo (<p>Hello {$who}, <em>welcome</em> back</p>)->__toString(), "\n";

// Leading/trailing blank lines and indentation around text collapse away.
echo (<p>
    Just some text.
</p>)->__toString(), "\n";

// Words on separate lines join with a single space.
echo (<p>
    one
    two
    three
</p>)->__toString(), "\n";

// Inline trailing space with no newline is kept (so adjacent elements don't fuse).
echo (<p>a <b>b</b> c</p>)->__toString(), "\n";

// --- childless elements: empty or self-closed are equivalent ---
// <div></div> and <div/> are equivalent - both allowed, so real HTML can be
// pasted into markup unchanged.
var_dump((string) <div></div> === (string) <div/>);
echo <div></div>, "\n";
echo <span class="a"></span>, "\n";

// A whitespace-only multi-line body normalizes away to the same empty element.
echo <div>
</div>, "\n";

// Void elements still serialize with no closing tag, from either source form.
echo <br/>, "\n";
echo <br></br>, "\n";

// --- self-closing source; serializer expands non-void elements ---
// Void elements serialize without a closing tag...
echo (<img/>)->__toString(), "\n";
echo (<img src="/a.png"/>)->__toString(), "\n";
// ...non-void self-closed source expands to open+close output.
echo (<div/>)->__toString(), "\n";
echo (<span class="x"/>)->__toString(), "\n";

// Empty components and fragments are allowed (only HTML elements must self-close).
class C implements Markup\Html {
    public function toHtml(): Markup\Html { return Markup\raw('C'); }
}
echo (<C></C>)->__toString(), "\n";
var_dump((<></>)->__toString());

// --- literal braces via character references or {'{'} ---
// { begins interpolation; a literal { is written as in HTML or JSX.

// Character references decode to literal braces at compile time (no interpolation).
echo (<p>&#123;x&#125;</p>)->__toString(), "\n";
echo (<p>&lbrace;x&rbrace;</p>)->__toString(), "\n";

// The JSX form: an interpolated string literal.
echo (<p>{'{'}x{'}'}</p>)->__toString(), "\n";

// A lone } in text is already literal.
echo (<p>a } b</p>)->__toString(), "\n";

// Literal JSON in a <script> uses the raw() island pattern (text children are
// HTML-escaped, and browsers never entity-decode inside raw-text elements).
echo (<script type="application/json" id="config">{Markup\raw(<<<'JSON'
{"@type": "Article", "title": "Braces & quotes stay verbatim"}
JSON)}</script>)->__toString(), "\n";

// --- comments: <!-- --> is literal output; {/* */} renders nothing ---
$x = 'ignored';

// <!-- --> is emitted verbatim; its content is literal (no interpolation).
echo (<div><!-- a literal {$x} comment --><p>body</p></div>)->__toString(), "\n";

// {/* ... */} is a source-only comment and renders nothing.
echo (<p>a{/* dropped */}b</p>)->__toString(), "\n";

// an empty interpolation renders nothing too
echo (<p>x{}y</p>)->__toString(), "\n";

// comment between block elements survives (it is real output, not whitespace)
echo (<ul>
    <!-- first -->
    <li>one</li>
</ul>)->__toString(), "\n";

// --- operand positions: markup lexes after every operand-expecting token ---
// (the scanner's allowlist; a comparison never silently changes meaning)
echo "concat " . <u>c</u>, "\n";                      // after "."
echo true ? <s>t</s> : <s>f</s>, "\n";                // after "?" and ":"
echo null ?? <em>coal</em>, "\n";                     // after "??"
echo (string) <p>cast</p>, "\n";                      // after a cast
echo <div>pipe</div> |> strtoupper(...), "\n";        // after "|>"
echo clone <span>cl</span>, "\n";                     // after `clone`
print <p>print</p>; echo "\n";                        // after `print`
echo [<b>a</b>, <b>b</b>][1], "\n";                   // after "[" and ","
echo (fn() => <li>arrow</li>)(), "\n";                // after "=>"
echo match(true) { true => <b>match</b> }, "\n";      // match arm "=>"
function gen() { yield from [<i>yf</i>]; }            // after `yield from`
echo iterator_to_array(gen())[0], "\n";
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
