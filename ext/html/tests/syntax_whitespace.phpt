--TEST--
Markup syntax: JSX-style whitespace normalization (RFC §7)
--EXTENSIONS--
html
--FILE--
<?php
// Newlines + indentation between block elements are dropped.
echo (<ul>
    <li>one</li>
    <li>two</li>
</ul>)->__toString(), "\n";

// A meaningful single space between inline content is preserved.
$name = 'Ada';
echo (<p>Hello {$name}, <em>welcome</em> back</p>)->__toString(), "\n";

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
?>
--EXPECT--
<ul><li>one</li><li>two</li></ul>
<p>Hello Ada, <em>welcome</em> back</p>
<p>Just some text.</p>
<p>one two three</p>
<p>a <b>b</b> c</p>
