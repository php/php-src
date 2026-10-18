--TEST--
OOM OOM in zend_call_function()
--INI--
memory_limit=10M
--ENV--
call_user_func=call_user_func
--FILE--
<?php

$a = [];
for ($i = 0; $i < 10; $i++) {
    /* Check that call_user_func() doesn't reset vm_interrupt without checking for OOMs */
    $a[] = getenv('call_user_func')('str_repeat', 'a', 10*1024*1024);
}

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 6
