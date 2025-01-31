--TEST--
Test whitespace following end of nowdoc
--INI--
highlight.string  = #DD0000
highlight.comment = #FF8000
highlight.keyword = #007700
highlight.default = #0000BB
highlight.html    = #000000
--WHITESPACE_SENSITIVE--
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
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php
  $x </span><span style="color: #007700">= &lt;&lt;&lt;'EOT'
</span><span style="color: #DD0000">some string    
</span><span style="color: #007700">EOT
  </span><span style="color: #0000BB">$y </span><span style="color: #007700">= </span><span style="color: #0000BB">2</span><span style="color: #007700">;
</span><span style="color: #0000BB">?&gt;</span></code></pre>
