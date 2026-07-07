--TEST--
Markup syntax: childless elements may be written empty or self-closed (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
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
?>
--EXPECT--
bool(true)
<div></div>
<span class="a"></span>
<div></div>
<br>
<br>
