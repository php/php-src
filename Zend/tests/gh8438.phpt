--TEST--
GH-8438 (HashTableIterators of empty hashtables are not poisoned upon destruction)
--EXTENSIONS--
zend_test
--FILE--
<?php

$destroyFunctions = ['zend_hash_destroy', 'zend_hash_graceful_reverse_destroy'];
$states = ['uninitialized', 'packed', 'packed_emptied', 'hash', 'hash_emptied'];

foreach ($destroyFunctions as $destroy) {
    foreach ($states as $state) {
        echo "$destroy, $state: ";
        var_dump(zend_test_hash_destroy_detaches_iterator($destroy, $state));
    }
}

?>
--EXPECT--
zend_hash_destroy, uninitialized: bool(true)
zend_hash_destroy, packed: bool(true)
zend_hash_destroy, packed_emptied: bool(true)
zend_hash_destroy, hash: bool(true)
zend_hash_destroy, hash_emptied: bool(true)
zend_hash_graceful_reverse_destroy, uninitialized: bool(true)
zend_hash_graceful_reverse_destroy, packed: bool(true)
zend_hash_graceful_reverse_destroy, packed_emptied: bool(true)
zend_hash_graceful_reverse_destroy, hash: bool(true)
zend_hash_graceful_reverse_destroy, hash_emptied: bool(true)
