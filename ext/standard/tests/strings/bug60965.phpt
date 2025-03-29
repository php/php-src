--TEST--
Bug #60965: Buffer overflow on htmlspecialchars/entities with $double=false
--FILE--
<?php
echo htmlspecialchars('"""""""""""""""""""""""""""""""""""""""""""""&#x123456789123456789123456789;',
ENT_QUOTES, 'UTF-8', false), "\n";
echo "Done.\n";
?>
--EXPECT--
&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&amp;#x123456789123456789123456789;
Done.
