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
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php </span><span style="color: #007700">echo </span><span style="color: #DD0000">"foo[] </span><span style="color: #0000BB">$a</span><span style="color: #DD0000"> \n"</span><span style="color: #007700">; </span><span style="color: #0000BB">?&gt;</span></code></pre>
