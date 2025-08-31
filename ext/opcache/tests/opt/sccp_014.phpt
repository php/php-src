--TEST--
SCCP 014: Conditional Constant Propagation of non-escaping object properties on PHI
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function loadEntities($entity_information) {
    $entity_types = new StdClass();
    $entity_types->a = 1;
    foreach ($entity_information as $info) {
        $entity_types->a = 0;
    }
    var_dump((bool)($entity_types->a));
}

loadEntities(array("first", "second"));
?>
--EXPECT--
bool(false)
