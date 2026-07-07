--TEST--
Markup syntax: <!-- --> is a literal comment; {/* */} renders nothing (RFC §7)
--EXTENSIONS--
html
--FILE--
<?php
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
?>
--EXPECT--
<div><!-- a literal {$x} comment --><p>body</p></div>
<p>ab</p>
<p>xy</p>
<ul><!-- first --><li>one</li></ul>
