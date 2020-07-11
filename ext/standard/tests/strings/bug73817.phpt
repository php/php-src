--TEST--
Bug #73817 (Incorrect entries in get_html_translation_table)
--FILE--
<?php
$entities = get_html_translation_table( HTML_ENTITIES, ENT_QUOTES | ENT_HTML5);
foreach ($entities as $entity) {
    if (substr($entity, -1) !== ';') {
        var_dump($entity);
    }
}
?>
===DONE===
--EXPECT--
===DONE===
