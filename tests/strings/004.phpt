--TEST--
highlight_string() buffering
--POST--
--GET--
--FILE--
<?php 
$var = highlight_string("<br /><?php echo \"foo\"; ?><br />");
$var = highlight_string("<br /><?php echo \"bar\"; ?><br />", TRUE);
echo "\n[$var]\n";
?>
--EXPECT--
<code><font color="#000000">
&lt;br /&gt;<font color="#0000CC">&lt;?php </font><font color="#006600">echo </font><font color="#CC0000">"foo"</font><font color="#006600">; </font><font color="#0000CC">?&gt;</font>&lt;br /&gt;</font>
</code>
[<code><font color="#000000">
&lt;br /&gt;<font color="#0000CC">&lt;?php </font><font color="#006600">echo </font><font color="#CC0000">"bar"</font><font color="#006600">; </font><font color="#0000CC">?&gt;</font>&lt;br /&gt;</font>
</code>]
