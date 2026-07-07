--TEST--
Markup syntax: elements, interpolation, nesting, self-close, fragments (RFC §7)
--EXTENSIONS--
html
--FILE--
<?php
$name = "Ada & <friends>";

// Element with interpolation and a nested element.
$el = <div>Hello {$name} <em>world</em></div>;
var_dump($el instanceof Html\Element);
echo $el->__toString(), "\n";

// Self-closing element -> clean void-element output.
echo (<br/>)->__toString(), "\n";
echo (<img/>)->__toString(), "\n";

// Fragment (no wrapper).
$frag = <><h1>Title</h1><p>Body {$name}</p></>;
var_dump($frag instanceof Html\Fragment);
echo $frag->__toString(), "\n";

// Interpolation can hold any expression; arrays flatten, null renders "".
$items = ['x', 'y'];
echo (<ul>{array_map(fn($i) => <li>{$i}</li>, $items)}</ul>)->__toString(), "\n";
echo (<p>{true ? 'yes' : null}</p>)->__toString(), "\n";
?>
--EXPECT--
bool(true)
<div>Hello Ada &amp; &lt;friends&gt; <em>world</em></div>
<br>
<img>
bool(true)
<h1>Title</h1><p>Body Ada &amp; &lt;friends&gt;</p>
<ul><li>x</li><li>y</li></ul>
<p>yes</p>
