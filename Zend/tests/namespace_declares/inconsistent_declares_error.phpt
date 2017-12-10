--TEST--
Using inconsistent namespace declares in the same file
--FILE--
<?php

namespace_declare('Foo', ['strict_types' => 1]);
namespace_declare('Bar', ['strict_types' => 0]);

eval(<<<'PHP'
namespace Foo;
namespace Bar;
PHP
);

?>
--EXPECTF--
Fatal error: Namespaced declares for namespace "Bar" are inconsistent with other namespaces in this file in %s on line %d
