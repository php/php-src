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
<code><span style="color: #000000">
<br />$x=&lt;&lt;&lt;DD<br />jhdsjkfhjdsh<br />DD<br />."";<br />$a=&lt;&lt;&lt;DDDD<br />jhdsjkfhjdsh<br />DDDD;<br /></span>
</code>
