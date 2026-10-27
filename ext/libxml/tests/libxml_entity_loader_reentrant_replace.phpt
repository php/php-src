--TEST--
libxml external entity loader replacement is safe against destructor reentrancy
--EXTENSIONS--
libxml
--FILE--
<?php

class D
{
    public function __destruct()
    {
        libxml_set_external_entity_loader(fn() => null);
    }
}

$d = new D();

$oldLoader = function () use ($d) {
};

unset($d);

libxml_set_external_entity_loader($oldLoader);
unset($oldLoader);

libxml_set_external_entity_loader(fn() => 1);

$loader = libxml_get_external_entity_loader();
var_dump($loader());

?>
--EXPECT--
NULL
