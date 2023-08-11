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
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php
  $x </code><code style="color: #007700">= &lt;&lt;&lt;'EOT'
</code><code style="color: #DD0000">some string    
</code><code style="color: #007700">EOT
  </code><code style="color: #0000BB">$y </code><code style="color: #007700">= </code><code style="color: #0000BB">2</code><code style="color: #007700">;
</code><code style="color: #0000BB">?&gt;</code></pre>
