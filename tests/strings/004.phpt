--TEST--
highlight_string() buffering
--INI--
highlight.string=#DD0000
highlight.comment=#FF9900
highlight.keyword=#007700
highlight.default=#0000BB
highlight.html=#000000
--FILE--
<?php
$var = highlight_string("<br /><?php echo \"foo\"; ?><br />");
$var = highlight_string("<br /><?php echo \"bar\"; ?><br />", TRUE);
echo "\n[$var]\n";
?>
--EXPECT--
<pre><code style="color: #000000">&lt;br /&gt;<span style="color: #0000BB">&lt;?php </span><span style="color: #007700">echo </span><span style="color: #DD0000">"foo"</span><span style="color: #007700">; </span><span style="color: #0000BB">?&gt;</span>&lt;br /&gt;</code></pre>
[<pre><code style="color: #000000">&lt;br /&gt;<span style="color: #0000BB">&lt;?php </span><span style="color: #007700">echo </span><span style="color: #DD0000">"bar"</span><span style="color: #007700">; </span><span style="color: #0000BB">?&gt;</span>&lt;br /&gt;</code></pre>]
