--TEST--
Using inconsistent namespace declares in the same file (including global namespace last)
--FILE--
<?php

namespace_declare('Foo', ['strict_types' => 1]);

eval(<<<'PHP'
namespace Foo {}
namespace {}
PHP
);

?>
--EXPECTF--
Fatal error: Namespaced declares for namespace "" are inconsistent with other namespaces in this file in %s on line %d
