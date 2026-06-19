--TEST--
Bug #60965: Buffer overflow on htmlspecialchars/entities with $double=false
--FILE--
<?php
echo htmlspecialchars('"""""""""""""""""""""""""""""""""""""""""""""&#x000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005;',
ENT_QUOTES, 'UTF-8', false), "\n";
echo htmlspecialchars('&#xFFFFFF;', ENT_QUOTES, 'UTF-8', false), "\n";
echo "Done.\n";
?>
--EXPECT--
&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&amp;#x000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005;
&amp;#xFFFFFF;
Done.
