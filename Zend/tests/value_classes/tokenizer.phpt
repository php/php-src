--TEST--
Value is tokenized as a modifier only before a class declaration
--EXTENSIONS--
tokenizer
--FILE--
<?php
$sources = [
    '<?php value class A {}',
    '<?php VALUE/**/readonly final class B {}',
    '<?php value // comment' . "\n" . 'class C {}',
    '<?php value # comment' . "\n" . 'class D {}',
    '<?php class value {}',
    '<?php function value() {} value();',
    '<?php value::class;',
    '<?php namespace value;',
    '<?php new value();',
    '<?php value readonly className;',
    '<?php value className;',
    '<?php $o->value;',
    '<?php $o?->value();',
    '<?php Value\\Name::class;',
    '<?php class C { const ?value value = null; }',
];
foreach ($sources as $source) {
    foreach (token_get_all($source) as $token) {
        if (is_array($token) && strtolower($token[1]) === 'value') {
            echo token_name($token[0]), "\n";
        }
    }
}
?>
--EXPECT--
T_VALUE
T_VALUE
T_VALUE
T_VALUE
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
T_STRING
