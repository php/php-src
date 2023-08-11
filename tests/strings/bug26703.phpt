--TEST--
Bug #26703 (Certain characters inside strings incorrectly treated as keywords)
--INI--
highlight.string=#DD0000
highlight.comment=#FF9900
highlight.keyword=#007700
highlight.default=#0000BB
highlight.html=#000000
--FILE--
<?php
    highlight_string('<?php echo "foo[] $a \n"; ?>');
?>
--EXPECT--
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php </code><code style="color: #007700">echo </code><code style="color: #DD0000">"foo[] </code><code style="color: #0000BB">$a</code><code style="color: #DD0000"> \n"</code><code style="color: #007700">; </code><code style="color: #0000BB">?&gt;</code></pre>
