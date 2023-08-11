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
<pre style="color: #000000">&lt;br /&gt;<code style="color: #0000BB">&lt;?php </code><code style="color: #007700">echo </code><code style="color: #DD0000">"foo"</code><code style="color: #007700">; </code><code style="color: #0000BB">?&gt;</code>&lt;br /&gt;</pre>
[<pre style="color: #000000">&lt;br /&gt;<code style="color: #0000BB">&lt;?php </code><code style="color: #007700">echo </code><code style="color: #DD0000">"bar"</code><code style="color: #007700">; </code><code style="color: #0000BB">?&gt;</code>&lt;br /&gt;</pre>]
