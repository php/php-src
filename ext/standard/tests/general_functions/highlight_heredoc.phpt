--TEST--
highlight_string() handling of heredoc
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
<code><font color="#000000">
<br />$x=&lt;&lt;&lt;DD<br />jhdsjkfhjdsh<br />DD<br />."";<br />$a=&lt;&lt;&lt;DDDD<br />jhdsjkfhjdsh<br />DDDD;<br /></font>
</code>
