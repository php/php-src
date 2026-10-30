--TEST--
Highlighting empty nowdoc
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
EOT
  $y = 2;
?>
EOF;
highlight_string($code);
?>
--EXPECT--
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php
  $x </span><span style="color: #007700">= &lt;&lt;&lt;'EOT'
EOT
  </span><span style="color: #0000BB">$y </span><span style="color: #007700">= </span><span style="color: #0000BB">2</span><span style="color: #007700">;
</span><span style="color: #0000BB">?&gt;</span></code></pre>
