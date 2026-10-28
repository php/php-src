--TEST--
OOM SplFixedArray::fromArray() 003
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64bit only"); ?>
--INI--
memory_limit=17M
--FILE--
<?php

$a = new SplFixedArray();
for ($i = 0; $i < 128*1024; $i++) {
    @$a->{$i} = 0;
}

$pad = str_repeat('a', max(16*1024*1024 - memory_get_usage(true) - 512*1024, 1));

$a->__wakeup();

?>
--EXPECTF--
Deprecated: Method SplFixedArray::__wakeup() is deprecated since 8.4, this method is obsolete, as serialization hooks are provided by __unserialize() and __serialize() in %s on line %d

Fatal error: Allowed memory size of 17825792 bytes exhausted %S(tried to allocate %d bytes) in %s on line 10
