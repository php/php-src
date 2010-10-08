--TEST--
Bug #53021 (Failure to convert numeric entities with ENT_NOQUOTES and ISO-8859-1)
--FILE--
<?php
var_dump(unpack("H*",html_entity_decode("&#233;", ENT_QUOTES, "ISO-8859-1")));
echo "double quotes variations:", "\n";
echo html_entity_decode("&quot;", ENT_NOQUOTES, 'UTF-8'), "\n";
echo html_entity_decode("&#34;", ENT_NOQUOTES, 'UTF-8'), "\n";
echo html_entity_decode("&quot;", ENT_QUOTES, 'UTF-8'), "\n";
echo html_entity_decode("&#34;", ENT_QUOTES, 'UTF-8'), "\n";
echo html_entity_decode("&quot;", ENT_COMPAT, 'UTF-8'), "\n";
echo html_entity_decode("&#34;", ENT_COMPAT, 'UTF-8'), "\n";

echo "\nsingle quotes variations:", "\n";
echo html_entity_decode("&#39;", ENT_NOQUOTES, 'UTF-8'), "\n";
echo html_entity_decode("&#39;", ENT_QUOTES, 'UTF-8'), "\n";
echo html_entity_decode("&#39;", ENT_COMPAT, 'UTF-8'), "\n";
--EXPECT--
array(1) {
  [1]=>
  string(2) "e9"
}
double quotes variations:
&quot;
&#34;
"
"
"
"

single quotes variations:
&#39;
'
&#39;
