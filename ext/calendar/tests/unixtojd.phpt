--TEST--
unixtojd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--ENV--
TZ=UTC
--FILE--
<?php
// this line has no impact on test output on Windows
putenv('TZ=UTC');
// getenv('TZ') returns 'UTC' here
// putenv (basic_functions.c) does call tzset() when the env var being put is 'TZ'
//      -adding a call direct to GetEnvironmentVariableA just before tzset() is called to check the value of 'TZ' returns 'UTC'
// putting a call to date_default_timezone_set() here doesn't help
//
// on Windows, the only thing that gets this test to pass is to put TZ=UTC in--ENV-- section
// 		-since putenv() is written to call tzset() when env var is TZ, I assume that putenv("TZ=UTC") is intended to work
//			and should work on all platforms(including Windows).
// easter_date.phpt passes
//		-doesn't use --ENV-- section
//		-uses --INI-- section with date.timezone=UTC
//		-uses putenv('TZ=UTC')
// date.timezone=UTC
//		-if omitted from easter_date.phpt, outputs DATE_TZ_ERRMSG warning
//			-easter_date() calls mktime() and localtime()
//			-whereas unixtojd(1000000000) calls localtime(1000000000)
//		-if omitted from unixtojd.phpt, does NOT output DATE_TZ_ERRMSG
//
// unixtojd() calls php_localtime_r() which for Pacific timezone systems, returns a time -8 hours
//		-this incorrect localtime is passed to the julian date conversion (GregorianToSDN) function which works (probably correctly)
//			but returns -1 day from expected because its input is -1 from expected

echo unixtojd(). "\n";
echo unixtojd(40000). "\n";
echo unixtojd(1000000000). "\n";
echo unixtojd(1152459009). "\n";
?>
--EXPECTF--
%d
2440588
2452162
2453926
