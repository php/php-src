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
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php </span><span style="color: #FF8000">/*some comment..</span></code></pre>
