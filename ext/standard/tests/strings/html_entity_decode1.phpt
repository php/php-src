--TEST--
html_entity_decode: Decoding of entities after invalid entities
--FILE--
<?php
$arr = array(
    "&",
    "&&amp;",
    "&&#x24;",
    "&#&amp;",
    "&#&#x24;",
    "&#x&amp;",
    "&#x&#x24;",
    "&#x1&amp;",
    "&#x1&#x24;",
    "&#x20&amp;",
    "&#x20&#x24;",
    "&#1&amp;",
    "&#1&#x24;",
    "&#20&amp;",
    "&#20&#x24;",
    "&a&amp;",
    "&a&#x24;",
    "&aa&amp;",
    "&aa&#x24;",
    "&aa;&amp;",
    "&aa;&#x24;",
    "&;&amp;",
    "&;&#x24;",
);

$i = 0;
foreach ($arr as $ent) {
    if ($i % 2 == 1) {
        if (($a = html_entity_decode($ent, ENT_QUOTES, 'UTF-8')) !=
                ($b = htmlspecialchars_decode($ent, ENT_QUOTES))) {
            echo "htmlspecialchars_decode <-> html_entity_decode inconsistency","\n",
                 "$b <-> $a","\n";
        }
    }
    echo html_entity_decode($ent, ENT_QUOTES, 'UTF-8'), "\n";
}
echo "Done.\n";
?>
--EXPECT--
&
&&
&$
&#&
&#$
&#x&
&#x$
&#x1&
&#x1$
&#x20&
&#x20$
&#1&
&#1$
&#20&
&#20$
&a&
&a$
&aa&
&aa$
&aa;&
&aa;$
&;&
&;$
Done.
