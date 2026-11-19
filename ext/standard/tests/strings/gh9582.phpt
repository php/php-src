--TEST--
GH-9582 (htmlspecialchars_decode() ENT_SUBSTITUTE flag has no effect)
--FILE--
<?php
echo (new ReflectionFunction(htmlspecialchars_decode(...)))->getParameters()[1], "\n";
echo (new ReflectionFunction(html_entity_decode(...)))->getParameters()[1], "\n";

$invalid = "\xC3&amp;&quot;&#039;";
var_dump(htmlspecialchars_decode($invalid) === htmlspecialchars_decode($invalid, ENT_QUOTES | ENT_SUBSTITUTE));
var_dump(html_entity_decode($invalid) === html_entity_decode($invalid, ENT_QUOTES | ENT_SUBSTITUTE));
?>
--EXPECT--
Parameter #1 [ <optional> int $flags = ENT_QUOTES | ENT_HTML401 ]
Parameter #1 [ <optional> int $flags = ENT_QUOTES | ENT_HTML401 ]
bool(true)
bool(true)
