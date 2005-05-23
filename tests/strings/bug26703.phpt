--TEST--
Bug #26703 (Certain characters inside strings incorrectly treated as keywords)
--INI--
highlight.string=#DD0000
highlight.comment=#FF9900
highlight.keyword=#007700
highlight.bg=#FFFFFF
highlight.default=#0000BB
highlight.html=#000000
--FILE--
<?php
	highlight_string('<?php echo "foo[] $a \n"; ?>');
?>
--EXPECT--
<code><font color="#000000">
<font color="#0000BB">&lt;?php&nbsp;</font><font color="#007700">echo&nbsp;</font><font color="#DD0000">"foo[]&nbsp;$a&nbsp;\n"</font><font color="#007700">;&nbsp;</font><font color="#0000BB">?&gt;</font>
</font>
</code>
