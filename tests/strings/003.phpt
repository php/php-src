--TEST--
HTML entities
--POST--
--GET--
--FILE--
<?php 
setlocale ("LC_CTYPE", "C");
echo htmlspecialchars ("<>\"&åÄ\n");
echo htmlentities ("<>\"&åÄ\n");
?>
--EXPECT--

&lt;&gt;&quot;&amp;åÄ
&lt;&gt;&quot;&amp;&aring;&Auml;
