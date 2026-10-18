--TEST--
Sleep functions process queued APCs on Windows
--EXTENSIONS--
ffi
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef(<<<'C'
    typedef void (__stdcall *PAPCFUNC)(uintptr_t);
    void * __stdcall GetCurrentThread(void);
    unsigned long __stdcall QueueUserAPC(PAPCFUNC callback, void *thread, uintptr_t data);
    unsigned long __stdcall SleepEx(unsigned long milliseconds, int alertable);
C, 'kernel32.dll');

$called = 0;
$callback = static function (int $data) use (&$called): void {
    $called = $data;
};

echo "usleep()\n";
var_dump($ffi->QueueUserAPC($callback, $ffi->GetCurrentThread(), 1) !== 0);
var_dump(usleep(1000000));
var_dump($called === 1);

echo "time_nanosleep()\n";
var_dump($ffi->QueueUserAPC($callback, $ffi->GetCurrentThread(), 2) !== 0);
$remaining = time_nanosleep(2, 0);
var_dump($called === 2);
var_dump(is_array($remaining));
if (is_array($remaining)) {
    var_dump($remaining['seconds'] >= 0 && $remaining['seconds'] <= 2);
    var_dump($remaining['nanoseconds'] >= 0 && $remaining['nanoseconds'] < 1000000000);
    var_dump($remaining['seconds'] > 0 || $remaining['nanoseconds'] > 0);
    var_dump($remaining['seconds'] < 2 || $remaining['nanoseconds'] === 0);
}

echo "time_sleep_until()\n";
var_dump($ffi->QueueUserAPC($callback, $ffi->GetCurrentThread(), 3) !== 0);
$deadline = microtime(true) + 0.1;
var_dump(time_sleep_until($deadline));
var_dump($called === 3);
var_dump(microtime(true) + 0.002 >= $deadline);

$ffi->SleepEx(0, 1);
?>
--EXPECT--
usleep()
bool(true)
NULL
bool(true)
time_nanosleep()
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
time_sleep_until()
bool(true)
bool(true)
bool(true)
bool(true)
