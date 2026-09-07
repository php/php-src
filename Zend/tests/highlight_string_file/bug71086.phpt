--TEST--
Bug #71086: Invalid numeric literal parse error within highlight_string() function
--FILE--
<?php

$highlightedString = highlight_string("<?php \n 09 09 09;", true);
var_dump($highlightedString);

?>
--EXPECT--
string(139) "<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php 
 09 09 09</span><span style="color: #007700">;</span></code></pre>"
