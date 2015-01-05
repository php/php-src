--TEST--
Test whitespace following end of nowdoc
--INI--
highlight.string  = #DD0000
highlight.comment = #FF8000
highlight.keyword = #007700
highlight.default = #0000BB
highlight.html    = #000000
--FILE--
<?php
$code = <<<'EOF'
<?php
  $x = <<<'EOT'
some string    
EOT
  $y = 2;
?>
EOF;
highlight_string($code);
?>
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php<br />&nbsp;&nbsp;$x&nbsp;</span><span style="color: #007700">=&nbsp;&lt;&lt;&lt;'EOT'<br /></span><span style="color: #DD0000">some&nbsp;string&nbsp;&nbsp;&nbsp;&nbsp;<br /></span><span style="color: #007700">EOT<br />&nbsp;&nbsp;</span><span style="color: #0000BB">$y&nbsp;</span><span style="color: #007700">=&nbsp;</span><span style="color: #0000BB">2</span><span style="color: #007700">;<br /></span><span style="color: #0000BB">?&gt;</span>
</span>
</code>
