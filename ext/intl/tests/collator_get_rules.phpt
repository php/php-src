--TEST--
Collator::getRules() returns collation tailoring rules as UTF-8
--EXTENSIONS--
intl
--FILE--
<?php
$root = new Collator('root');
var_dump($root->getRules());
var_dump(collator_get_rules($root));

$collator = new Collator('de_DE@collation=phonebook');
$rules = $collator->getRules();
var_dump(is_string($rules) && $rules !== '');
var_dump(preg_match('/\x{00E4}/u', $rules));
var_dump(collator_get_rules($collator) === $rules);

unset($collator);
var_dump(preg_match('/\x{00E4}/u', $rules));
?>
--EXPECT--
string(0) ""
string(0) ""
bool(true)
int(1)
bool(true)
int(1)
