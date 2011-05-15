--TEST--
HTML entities of ISO-8859 chars
--INI--
output_handler=
--FILE--
<?php 
setlocale (LC_CTYPE, "C");
$sc_encoded = htmlspecialchars ("<>\"&åÄ\n",ENT_COMPAT,"ISO-8859-1");
echo $sc_encoded;
$ent_encoded = htmlentities ("<>\"&åÄ\n",ENT_COMPAT,"ISO-8859-1");
echo $ent_encoded;
echo html_entity_decode($sc_encoded,ENT_COMPAT,"ISO-8859-1");
echo html_entity_decode($ent_encoded,ENT_COMPAT,"ISO-8859-1");
?>
--EXPECT--
&lt;&gt;&quot;&amp;åÄ
&lt;&gt;&quot;&amp;&aring;&Auml;
<>"&åÄ
<>"&åÄ
