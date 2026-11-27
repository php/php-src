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
$callconv = PHP_INT_SIZE === 4 ? '__stdcall' : '';
// kernel32.dll exports undecorated names: resolve them with the default
// calling convention, then cast them to their actual calling convention
$kernel32 = FFI::cdef(<<<C
    void *GetCurrentThread(void);
    unsigned long QueueUserAPC(void *callback, void *thread, uintptr_t data);
    unsigned long SleepEx(unsigned long milliseconds, int alertable);
C, 'kernel32.dll');
$ffi = FFI::cdef(<<<C
    typedef void $callconv (*PAPCFUNC)(uintptr_t);
    typedef void *$callconv (*GetCurrentThread_t)(void);
    typedef unsigned long $callconv (*QueueUserAPC_t)(PAPCFUNC callback, void *thread, uintptr_t data);
    typedef unsigned long $callconv (*SleepEx_t)(unsigned long milliseconds, int alertable);
C);
$GetCurrentThread = $ffi->cast('GetCurrentThread_t', $kernel32->GetCurrentThread);
$QueueUserAPC = $ffi->cast('QueueUserAPC_t', $kernel32->QueueUserAPC);
$SleepEx = $ffi->cast('SleepEx_t', $kernel32->SleepEx);

$called = 0;
$callback = static function (int $data) use (&$called): void {
    $called = $data;
    // The time spent in APCs is not time slept
    usleep(300000);
};

echo "usleep()\n";
var_dump($QueueUserAPC($callback, $GetCurrentThread(), 1) !== 0);
var_dump(usleep(1000000));
var_dump($called === 1);

echo "time_nanosleep()\n";
var_dump($QueueUserAPC($callback, $GetCurrentThread(), 2) !== 0);
var_dump(time_nanosleep(2, 0));
var_dump($called === 2);

echo "time_sleep_until()\n";
var_dump($QueueUserAPC($callback, $GetCurrentThread(), 3) !== 0);
$deadline = microtime(true) + 0.5;
var_dump(time_sleep_until($deadline));
var_dump($called === 3);
$now = microtime(true);
var_dump($now + 0.002 >= $deadline);
var_dump($now < $deadline + 0.25);

$SleepEx(0, 1);
?>
--EXPECT--
usleep()
bool(true)
NULL
bool(true)
time_nanosleep()
bool(true)
array(2) {
  ["seconds"]=>
  int(2)
  ["nanoseconds"]=>
  int(0)
}
bool(true)
time_sleep_until()
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
