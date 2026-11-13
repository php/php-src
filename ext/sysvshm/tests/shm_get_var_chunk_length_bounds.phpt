--TEST--
shm_get_var() must not trust the chunk length stored in a hostile segment for unserialize()
--EXTENSIONS--
sysvshm
ffi
--INI--
ffi.enable=1
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') {
    die('skip shmget is not available to FFI on this OS');
}
if (PHP_INT_SIZE !== 8) {
    die('skip FFI chunk layout assumes 64-bit zend_long');
}
?>
--FILE--
<?php

function craft_hostile_segment(int $key, int $len): void {
    $ffi = FFI::cdef("
        typedef struct { char magic[6]; long start; long end; long free_; long total; } head_t;
        typedef struct { long key; long length; long next; char mem[16]; } chunk_t;
        int shmget(int, int, int);
        void *shmat(int, const void *, int);
    ");
    $id = $ffi->shmget($key, 4096, 0666 | 01000);
    if ($id < 0) {
        echo "shm setup failed\n";
        return;
    }
    $p = $ffi->shmat($id, NULL, 0);
    if ($p == $ffi->cast('char*', -1)) {
        echo "shmat failed\n";
        return;
    }
    FFI::memset($p, 0, 4096);
    $head = $ffi->cast('head_t*', $p);
    FFI::memcpy($head->magic, "PHP_SM", 6);
    $head->start = 40;
    $head->end = 4096;
    $head->free_ = 0;
    $head->total = 4096;
    $chunk = $ffi->cast('chunk_t*', $ffi->cast('char*', $p) + 40);
    $chunk->key = 1;
    $chunk->length = $len;
    $chunk->next = 4096 - 40;
    FFI::memcpy($chunk->mem, "i:42;", 5);
}

$key1 = 0x5A5A0E01;
$key2 = 0x5A5A0E02;

$old = @shm_attach($key1);
if ($old !== false) {
    shm_remove($old);
}
$old = @shm_attach($key2);
if ($old !== false) {
    shm_remove($old);
}
craft_hostile_segment($key1, PHP_INT_MAX);

$shm = shm_attach($key1, 4096);
var_dump(shm_has_var($shm, 1));
var_dump(shm_get_var($shm, 1));
shm_remove($shm);

$shm2 = shm_attach($key2, 4096);
shm_put_var($shm2, 1, 42);
var_dump(shm_get_var($shm2, 1));
shm_remove($shm2);

echo "Done\n";
?>
--EXPECTF--
bool(true)

Warning: shm_get_var(): Variable data in shared memory is corrupted in %s on line %d
bool(false)
int(42)
Done
