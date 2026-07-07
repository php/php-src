--TEST--
Markup syntax: self-closing source; serializer expands non-void elements (RFC §7)
--EXTENSIONS--
html
--FILE--
<?php
// Void elements serialize without a closing tag...
echo (<br/>)->__toString(), "\n";
echo (<img src="/a.png"/>)->__toString(), "\n";
// ...non-void self-closed source expands to open+close output.
echo (<div/>)->__toString(), "\n";
echo (<span class="x"/>)->__toString(), "\n";

// Empty components and fragments are allowed (only HTML elements must self-close).
function C(): Html\Htmlable { return Html\raw('C'); }
echo (<C></C>)->__toString(), "\n";
var_dump((<></>)->__toString());
?>
--EXPECT--
<br>
<img src="/a.png">
<div></div>
<span class="x"></span>
C
string(0) ""
