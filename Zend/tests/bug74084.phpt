--TEST--
Bug #74084 (Out of bound read - zend_mm_alloc_small)
--INI--
error_reporting=0
--FILE--
<?php
$$A += $$B->a = &$$C; 
unset($$A);
$$A -= $$B->a = &$$C; 
unset($$A);
$$A *= $$B->a = &$$C; 
unset($$A);
$$A /= $$B->a = &$$C; 
unset($$A);
$$A **= $$B->a = &$$C; 
var_dump($$A);
?>
--EXPECT--
int(1)
