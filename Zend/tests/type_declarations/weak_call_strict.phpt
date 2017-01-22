--TEST--
strict_types=0 code calling strict_types=1 code
--FILE--
<?php

// implicitly strict_types=0

// file with strict_types=1
require 'weak_call_strict_2.inc';

// Will succeed: Function was declared in strict mode, but that does not matter
// This file uses weak mode, so the call is weak, and float accepted for int
function_declared_in_strict_mode(1.0);
?>
--EXPECT--
Success!
