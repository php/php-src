--TEST--
FFI Thread safe callbacks
--EXTENSIONS--
ffi
--SKIPIF--
<?php
try {
	$libc = FFI::cdef('
		int pthread_create(void *restrict thread,
							const void *restrict attr,
							void *(*start_routine)(void *),
							void *arg);
		', 'libc.so.6');
} catch(Throwable $_){
	die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$libc = FFI::cdef('
typedef uint64_t pthread_t;
int pthread_create(pthread_t *thread,
					const void* attr,
					void *(*start_routine)(void *),
					void *arg);
int pthread_detach(pthread_t thread);
', 'libc.so.6');

$tid = $libc->new($libc->type('pthread_t'));
$accum = 0;
$thread_func = function($arg) use($libc, &$accum){
	//$v = $libc->cast('int *', $arg)[0];
	FFI::free($arg);
	usleep(10 * 1000);
	$accum++;
	//print(".");
};

for($i=0; $i<100; $i++){
	$arg = $libc->new('int', false);
	$arg->cdata = $i;

	$libc->pthread_create(
		FFI::addr($tid), NULL,
		$thread_func, FFI::addr($arg)
	);
	$libc->pthread_detach($tid->cdata);
}

while($accum != 100){
	//print("w");
	usleep(1000);
}
print($accum);
?>
--EXPECT--
100