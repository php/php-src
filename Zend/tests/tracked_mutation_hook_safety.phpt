--TEST--
Tracked mutation hooks are called through defensive helpers
--SKIPIF--
<?php
$zendDir = dirname(__DIR__);
foreach (['zend_execute.h', 'zend_execute.c', 'zend_vm_def.h', 'zend_vm_execute.h'] as $file) {
	if (!is_file($zendDir . '/' . $file)) {
		die('skip source tree required');
	}
}
?>
--FILE--
<?php

function sourceSection(string $source, string $start, string $end, string $label): string
{
	$startPos = strpos($source, $start);
	$endPos = strpos($source, $end, $startPos === false ? 0 : $startPos);

	if ($startPos === false || $endPos === false || $endPos <= $startPos) {
		throw new RuntimeException('Unable to locate ' . $label);
	}

	return substr($source, $startPos, $endPos - $startPos);
}

$zendDir = dirname(__DIR__);
$executeH = file_get_contents($zendDir . '/zend_execute.h');
$executeC = file_get_contents($zendDir . '/zend_execute.c');

if (!str_contains($executeH, 'static zend_always_inline bool zend_maybe_track_hash_mutation(HashTable *ht, bool publish)')) {
	throw new RuntimeException('Missing hash mutation helper');
}

if (!str_contains($executeH, 'zend_tracked_hash_mutation_hook != NULL')) {
	throw new RuntimeException('Hash mutation helper does not guard the hook pointer');
}

foreach (['zend_execute.c', 'zend_vm_def.h', 'zend_vm_execute.h'] as $file) {
	$source = file_get_contents($zendDir . '/' . $file);
	if (str_contains($source, 'zend_tracked_hash_mutation_hook(')) {
		throw new RuntimeException($file . ' calls zend_tracked_hash_mutation_hook() directly');
	}
}

if (!str_contains($executeC, 'zobj != NULL && zend_tracked_object_mutation_hook != NULL && EG(exception) == NULL')) {
	throw new RuntimeException('Object mutation slow path does not guard the hook pointer');
}

if (!str_contains($executeC, 'zobj != NULL && zend_tracked_object_mutation_hook != NULL && EG(exception) == NULL && value != &EG(error_zval)')) {
	throw new RuntimeException('Object mutation with-value slow path does not guard the hook pointer');
}

$assign = sourceSection(
	$executeH,
	'static zend_always_inline zval* zend_assign_to_variable(',
	'static zend_always_inline zval* zend_assign_to_variable_ex(',
	'zend_assign_to_variable()'
);
if (substr_count($assign, 'zend_maybe_track_reference_update(updated_ref);') < 2) {
	throw new RuntimeException('zend_assign_to_variable() does not notify reference updates');
}

$assignEx = sourceSection(
	$executeH,
	'static zend_always_inline zval* zend_assign_to_variable_ex(',
	'static zend_always_inline void zend_class_static_update(',
	'zend_assign_to_variable_ex()'
);
if (substr_count($assignEx, 'zend_maybe_track_reference_update(updated_ref);') < 2) {
	throw new RuntimeException('zend_assign_to_variable_ex() does not notify reference updates');
}

echo "OK\n";

?>
--EXPECT--
OK
