--TEST--
highlight_string() handling of heredoc
--INI--
highlight.html=#000000
--FILE--
<?php
$str = '
$x=<<<DD
jhdsjkfhjdsh
DD
."";
$a=<<<DDDD
jhdsjkfhjdsh
DDDD;
';
highlight_string($str);
?>
--EXPECT--
<pre><code style="color: #000000">
$x=&lt;&lt;&lt;DD
jhdsjkfhjdsh
DD
."";
$a=&lt;&lt;&lt;DDDD
jhdsjkfhjdsh
DDDD;
</code></pre>
