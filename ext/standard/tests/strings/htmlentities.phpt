--TEST--
HTML entities
--INI--
output_handler=
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
--FILE--
<?php 
setlocale (LC_CTYPE, "C");
$sc_encoded = htmlspecialchars ("<>\"&åÄ\n");
echo $sc_encoded;
$ent_encoded = htmlentities ("<>\"&åÄ\n");
echo $ent_encoded;
echo html_entity_decode($sc_encoded);
echo html_entity_decode($ent_encoded);
?>
--EXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
&lt;&gt;&quot;&amp;åÄ
&lt;&gt;&quot;&amp;&aring;&Auml;
<>"&åÄ
<>"&åÄ
