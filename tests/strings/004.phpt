--TEST--
highlight_string() buffering
--POST--
--INI--
highlight.string=#DD0000
highlight.comment=#FF9900
highlight.keyword=#007700
highlight.bg=#FFFFFF
highlight.default=#0000BB
highlight.html=#000000
--GET--
--FILE--
<?php 
$var = highlight_string("<br /><?php echo \"foo\"; ?><br />");
$var = highlight_string("<br /><?php echo \"bar\"; ?><br />", TRUE);
echo "\n[$var]\n";
?>
--EXPECT--
<code><font color="#000000">
&lt;br&nbsp;/&gt;<font color="#0000BB">&lt;?php&nbsp;</font><font color="#007700">echo&nbsp;</font><font color="#DD0000">"foo"</font><font color="#007700">;&nbsp;</font><font color="#0000BB">?&gt;</font>&lt;br&nbsp;/&gt;</font>
</code>
[<code><font color="#000000">
&lt;br&nbsp;/&gt;<font color="#0000BB">&lt;?php&nbsp;</font><font color="#007700">echo&nbsp;</font><font color="#DD0000">"bar"</font><font color="#007700">;&nbsp;</font><font color="#0000BB">?&gt;</font>&lt;br&nbsp;/&gt;</font>
</code>]
