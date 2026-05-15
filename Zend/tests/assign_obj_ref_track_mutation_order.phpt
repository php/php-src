--TEST--
ASSIGN_OBJ_REF tracks object mutations before freeing operands
--SKIPIF--
<?php
$zendDir = dirname(__DIR__);
if (!is_file($zendDir . '/zend_vm_def.h') || !is_file($zendDir . '/zend_vm_execute.h')) {
	die('skip source tree required');
}
?>
--FILE--
<?php

function assertAssignObjRefTrackingOrder(string $section, string $label): bool
{
	$trackPos = strpos($section, 'ZEND_MAYBE_TRACK_OBJECT_MUTATION(zobj);');
	$freeNeedles = [
		'FREE_OP1();',
		'FREE_OP2();',
		'FREE_OP_DATA();',
		'zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));',
		'zval_ptr_dtor_nogc(EX_VAR(opline->op2.var));',
		'zval_ptr_dtor_nogc(EX_VAR((opline+1)->op1.var));',
	];
	$freePositions = [];

	foreach ($freeNeedles as $needle) {
		$pos = strpos($section, $needle);
		if ($pos !== false) {
			$freePositions[] = $pos;
		}
	}

	if ($trackPos === false) {
		throw new RuntimeException($label . ' is missing the expected ASSIGN_OBJ_REF sequence');
	}

	if ($freePositions === []) {
		return false;
	}

	if ($trackPos > min($freePositions)) {
		throw new RuntimeException($label . ' tracks object mutation after freeing operands');
	}

	return true;
}

$zendDir = dirname(__DIR__);

$vmDef = file_get_contents($zendDir . '/zend_vm_def.h');
$defStart = strpos($vmDef, 'ZEND_VM_HANDLER(32, ZEND_ASSIGN_OBJ_REF');
$defEnd = strpos($vmDef, 'ZEND_VM_HANDLER(33, ZEND_ASSIGN_STATIC_PROP_REF');
if ($defStart === false || $defEnd === false) {
	throw new RuntimeException('Unable to locate ZEND_ASSIGN_OBJ_REF in zend_vm_def.h');
}

if (!assertAssignObjRefTrackingOrder(substr($vmDef, $defStart, $defEnd - $defStart), 'zend_vm_def.h')) {
	throw new RuntimeException('zend_vm_def.h ASSIGN_OBJ_REF unexpectedly has no operand frees');
}

$vmExecute = file_get_contents($zendDir . '/zend_vm_execute.h');
$sections = preg_split('/(?=static ZEND_OPCODE_HANDLER_RET )/', $vmExecute);
$checked = 0;

foreach ($sections as $section) {
	$headerEnd = strpos($section, "\n");
	$label = $headerEnd === false ? 'zend_vm_execute.h ASSIGN_OBJ_REF handler' : trim(substr($section, 0, $headerEnd));
	if (!preg_match('/\bZEND_ASSIGN_OBJ_REF_[A-Z0-9_]+_HANDLER\b/', $label)) {
		continue;
	}

	if (assertAssignObjRefTrackingOrder($section, $label)) {
		$checked++;
	}
}

if ($checked === 0) {
	throw new RuntimeException('Unable to locate generated ZEND_ASSIGN_OBJ_REF handlers in zend_vm_execute.h');
}

echo "OK\n";

?>
--EXPECT--
OK
