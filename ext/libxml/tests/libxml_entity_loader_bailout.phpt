--TEST--
GH-24763: libxml external entity loader is detached before callback destruction bailout
--EXTENSIONS--
libxml
zend_test
--FILE--
<?php

class Bailout
{
    public function __destruct()
    {
        zend_trigger_bailout();
    }
}

$bailout = new Bailout();

$loader = function () use ($bailout) {
};

unset($bailout);

register_shutdown_function(function () {
    var_dump(libxml_get_external_entity_loader());
});

libxml_set_external_entity_loader($loader);
unset($loader);

libxml_set_external_entity_loader(null);

echo "UNREACHABLE\n";

?>
--EXPECTF--
Fatal error: Bailout in %s on line %d
NULL
