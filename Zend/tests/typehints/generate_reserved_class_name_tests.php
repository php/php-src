<?php declare(strict_types=1);

// Generates test cases for reserved class/interface/trait names

const BASIC_TEMPLATE = <<<'PHPT'
--TEST--
'$name' cannot be used as a class, interface or trait name - $type
--FILE--
$body
--EXPECTF--
Fatal error: Cannot use '$name' as class name as it is reserved in %s on line %d
PHPT;

const CLASS_BODY_TEMPLATE = <<<'PHP'
<?php

$classType $name {}
PHP;

const CLASS_ALIAS_BODY_TEMPLATE = <<<'PHP'
<?php

class foobar {}
class_alias('foobar', '$name');
PHP;

const USE_TEMPLATE = <<<'PHPT'
--TEST--
'$name' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as $name;
--EXPECTF--
Fatal error: Cannot use foobar as $name because '$name' is a special class name in %s on line %d
PHPT;

const RESERVED_NAMES = [
    'bool',
    'false',
    'float',
    'int',
    'null',
    'parent',
    'self',
    'static',
    'string',
    'true',
    'integer',
    'long',
    'double',
    'boolean'
];

const CLASS_KINDS = [
    'class',
    'interface',
    'trait'
];

foreach (RESERVED_NAMES as $reserved) {
    // "static" is weird and produces a different error
    if ($reserved !== "static") {
        foreach (CLASS_KINDS as $kind) {
            // class int {}, interface int {}, trait int {}
            $body = CLASS_BODY_TEMPLATE;
            $body = str_replace('$name', $reserved, $body);
            $body = str_replace('$classType', $kind, $body);

            $test = BASIC_TEMPLATE;
            $test = str_replace('$name', $reserved, $test);
            $test = str_replace('$type', $kind, $test);
            $test = str_replace('$body', $body, $test);
            
            echo "Generated reserved_class_name_${reserved}_${kind}.phpt", PHP_EOL;
            file_put_contents("reserved_class_name_${reserved}_${kind}.phpt", $test);
        }

        // use ... as int; 
        $test = USE_TEMPLATE;
        $test = str_replace('$name', $reserved, $test);

        file_put_contents("reserved_class_name_${reserved}_use.phpt", $test);
        echo "Generated reserved_class_name_${reserved}_use.phpt", PHP_EOL;
    }

    // class_alias(..., 'int');
    $body = CLASS_ALIAS_BODY_TEMPLATE;
    $body = str_replace('$name', $reserved, $body);

    $test = BASIC_TEMPLATE;
    $test = str_replace('$name', $reserved, $test);
    $test = str_replace('$type', 'class_alias', $test);
    $test = str_replace('$body', $body, $test);

    file_put_contents("reserved_class_name_${reserved}_class_alias.phpt", $test);
    echo "Generated reserved_class_name_${reserved}_class_alias.phpt", PHP_EOL;
}
