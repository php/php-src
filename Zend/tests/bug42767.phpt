--TEST--
Bug #42767 (highlight_string() truncates trailing comments)
--INI--
highlight.string  = #DD0000
highlight.comment = #FF8000
highlight.keyword = #007700
highlight.default = #0000BB
highlight.html    = #000000
--FILE--
<?php
highlight_string('<?php /*some comment..');
?>
--EXPECT--
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php </code><code style="color: #FF8000">/*some comment..</code></pre>
