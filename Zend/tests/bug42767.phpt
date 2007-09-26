--TEST--
Bug #42767 (highlight_string() truncates trailing comments)
--FILE--
<?php
highlight_string('<?php /*some comment..');
?>
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #FF8000">/*some&nbsp;comment..</span>
</span>
</code>
