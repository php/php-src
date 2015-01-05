--TEST--
abusing preg_match_all() #2
--FILE--
<?php
// this file is not used in the cron job
// use it to test the gcc regex with the sample data provided

$sampledata = "
/p2/var/php_gcov/PHP_4_4/ext/ming/ming.c: In function `zif_swfbitmap_init':
/p2/var/php_gcov/PHP_4_4/ext/ming/ming.c:323: warning: assignment from incompatible pointer type
/p2/var/php_gcov/PHP_4_4/ext/ming/ming.c: In function `zif_swftextfield_setFont':
/p2/var/php_gcov/PHP_4_4/ext/ming/ming.c:2597: warning: passing arg 2 of `SWFTextField_setFont' from incompatible pointer type
/p2/var/php_gcov/PHP_4_4/ext/oci8/oci8.c:1027: warning: `oci_ping' defined but not used
/p2/var/php_gcov/PHP_4_4/ext/posix/posix.c: In function `zif_posix_getpgid':
/p2/var/php_gcov/PHP_4_4/ext/posix/posix.c:484: warning: implicit declaration of function `getpgid'
/p2/var/php_gcov/PHP_4_4/ext/posix/posix.c: In function `zif_posix_getsid':
/p2/var/php_gcov/PHP_4_4/ext/posix/posix.c:506: warning: implicit declaration of function `getsid'
/p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c: In function `ps_read_files':
/p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c:302: warning: implicit declaration of function `pread'
/p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c: In function `ps_write_files':
/p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c:340: warning: implicit declaration of function `pwrite'
/p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c: In function `zif_socket_get_option':
/p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c:1862: warning: unused variable `timeout'
/p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c: In function `zif_socket_set_option':
/p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c:1941: warning: unused variable `timeout'
/p2/var/php_gcov/PHP_4_4/regex/regexec.c:19: warning: `nope' defined but not used
/p2/var/php_gcov/PHP_4_4/ext/standard/exec.c:50: warning: `php_make_safe_mode_command' defined but not used
/p2/var/php_gcov/PHP_4_4/ext/standard/image.c: In function `php_handle_jpc':
/p2/var/php_gcov/PHP_4_4/ext/standard/image.c:604: warning: unused variable `dummy_int'
/p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c: In function `php_gd_parse':
/p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c:1138: warning: implicit declaration of function `php_gd_lex'
/p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.y: At top level:
/p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.y:864: warning: return type defaults to `int'
/p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c: In function `zif_msg_receive':
/p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c:318: warning: passing arg 2 of `php_var_unserialize' from incompatible pointer type
/p2/var/php_gcov/PHP_4_4/ext/yp/yp.c: In function `zif_yp_err_string':
/p2/var/php_gcov/PHP_4_4/ext/yp/yp.c:372: warning: assignment discards qualifiers from pointer target type
Zend/zend_language_scanner.c:5944: warning: `yy_fatal_error' defined but not used
Zend/zend_language_scanner.c:2627: warning: `yy_last_accepting_state' defined but not used
Zend/zend_language_scanner.c:2628: warning: `yy_last_accepting_cpos' defined but not used
Zend/zend_language_scanner.c:2634: warning: `yy_more_flag' defined but not used
Zend/zend_language_scanner.c:2635: warning: `yy_more_len' defined but not used
Zend/zend_language_scanner.c:5483: warning: `yyunput' defined but not used
Zend/zend_language_scanner.c:5929: warning: `yy_top_state' defined but not used
conflicts: 2 shift/reduce
Zend/zend_ini_scanner.c:457: warning: `yy_last_accepting_state' defined but not used
Zend/zend_ini_scanner.c:458: warning: `yy_last_accepting_cpos' defined but not used
Zend/zend_ini_scanner.c:1361: warning: `yyunput' defined but not used
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c: In function `_safe_emalloc':
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c:237: warning: long int format, size_t arg (arg 3)
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c:237: warning: long int format, size_t arg (arg 4)
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c:237: warning: long int format, size_t arg (arg 5)
/p2/var/php_gcov/PHP_4_4/Zend/zend_ini.c:338: warning: `zend_ini_displayer_cb' defined but not used
ext/mysql/libmysql/my_tempnam.o(.text+0x80): In function `my_tempnam':
/p2/var/php_gcov/PHP_4_4/ext/mysql/libmysql/my_tempnam.c:115: warning: the use of `tempnam' is dangerous, better use `mkstemp'
ext/mysql/libmysql/my_tempnam.o(.text+0x80): In function `my_tempnam':
/p2/var/php_gcov/PHP_4_4/ext/mysql/libmysql/my_tempnam.c:115: warning: the use of `tempnam' is dangerous, better use `mkstemp'
ext/ming/ming.o(.text+0xc115): In function `zim_swfmovie_namedAnchor':
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:2207: undefined reference to `SWFMovie_namedAnchor'
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:2209: undefined reference to `SWFMovie_xpto'
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:2259: undefined reference to `SWFMovie_foo'
ext/ming/ming.o(.text+0x851): In function `zif_ming_setSWFCompression':
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:154: undefined reference to `Ming_setSWFCompression'
";

	// Regular expression to select the error and warning information
	// tuned for gcc 3.4, 4.0 and 4.1
	$gcc_regex = '/^((.+)(\(\.text\+0x[[:xdigit:]]+\))?: In function [`\'](\w+)\':\s+)?'.
		'((?(1)(?(3)[^:\n]+|\2)|[^:\n]+)):(\d+): (?:(error|warning):\s+)?(.+)'.
		str_repeat('(?:\s+\5:(\d+): (?:(error|warning):\s+)?(.+))?', 99). // capture up to 100 errors
		'/mS';


var_dump(preg_match_all($gcc_regex, $sampledata, $m, PREG_SET_ORDER));
print_r($m);

?>
--EXPECT--
int(24)
Array
(
    [0] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c: In function `zif_swfbitmap_init':
/p2/var/php_gcov/PHP_4_4/ext/ming/ming.c:323: warning: assignment from incompatible pointer type
            [1] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c: In function `zif_swfbitmap_init':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c
            [3] => 
            [4] => zif_swfbitmap_init
            [5] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c
            [6] => 323
            [7] => warning
            [8] => assignment from incompatible pointer type
        )

    [1] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c: In function `zif_swftextfield_setFont':
/p2/var/php_gcov/PHP_4_4/ext/ming/ming.c:2597: warning: passing arg 2 of `SWFTextField_setFont' from incompatible pointer type
            [1] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c: In function `zif_swftextfield_setFont':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c
            [3] => 
            [4] => zif_swftextfield_setFont
            [5] => /p2/var/php_gcov/PHP_4_4/ext/ming/ming.c
            [6] => 2597
            [7] => warning
            [8] => passing arg 2 of `SWFTextField_setFont' from incompatible pointer type
        )

    [2] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/oci8/oci8.c:1027: warning: `oci_ping' defined but not used
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => /p2/var/php_gcov/PHP_4_4/ext/oci8/oci8.c
            [6] => 1027
            [7] => warning
            [8] => `oci_ping' defined but not used
        )

    [3] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c: In function `zif_posix_getpgid':
/p2/var/php_gcov/PHP_4_4/ext/posix/posix.c:484: warning: implicit declaration of function `getpgid'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c: In function `zif_posix_getpgid':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c
            [3] => 
            [4] => zif_posix_getpgid
            [5] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c
            [6] => 484
            [7] => warning
            [8] => implicit declaration of function `getpgid'
        )

    [4] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c: In function `zif_posix_getsid':
/p2/var/php_gcov/PHP_4_4/ext/posix/posix.c:506: warning: implicit declaration of function `getsid'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c: In function `zif_posix_getsid':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c
            [3] => 
            [4] => zif_posix_getsid
            [5] => /p2/var/php_gcov/PHP_4_4/ext/posix/posix.c
            [6] => 506
            [7] => warning
            [8] => implicit declaration of function `getsid'
        )

    [5] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c: In function `ps_read_files':
/p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c:302: warning: implicit declaration of function `pread'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c: In function `ps_read_files':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c
            [3] => 
            [4] => ps_read_files
            [5] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c
            [6] => 302
            [7] => warning
            [8] => implicit declaration of function `pread'
        )

    [6] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c: In function `ps_write_files':
/p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c:340: warning: implicit declaration of function `pwrite'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c: In function `ps_write_files':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c
            [3] => 
            [4] => ps_write_files
            [5] => /p2/var/php_gcov/PHP_4_4/ext/session/mod_files.c
            [6] => 340
            [7] => warning
            [8] => implicit declaration of function `pwrite'
        )

    [7] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c: In function `zif_socket_get_option':
/p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c:1862: warning: unused variable `timeout'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c: In function `zif_socket_get_option':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c
            [3] => 
            [4] => zif_socket_get_option
            [5] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c
            [6] => 1862
            [7] => warning
            [8] => unused variable `timeout'
        )

    [8] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c: In function `zif_socket_set_option':
/p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c:1941: warning: unused variable `timeout'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c: In function `zif_socket_set_option':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c
            [3] => 
            [4] => zif_socket_set_option
            [5] => /p2/var/php_gcov/PHP_4_4/ext/sockets/sockets.c
            [6] => 1941
            [7] => warning
            [8] => unused variable `timeout'
        )

    [9] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/regex/regexec.c:19: warning: `nope' defined but not used
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => /p2/var/php_gcov/PHP_4_4/regex/regexec.c
            [6] => 19
            [7] => warning
            [8] => `nope' defined but not used
        )

    [10] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/standard/exec.c:50: warning: `php_make_safe_mode_command' defined but not used
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => /p2/var/php_gcov/PHP_4_4/ext/standard/exec.c
            [6] => 50
            [7] => warning
            [8] => `php_make_safe_mode_command' defined but not used
        )

    [11] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/standard/image.c: In function `php_handle_jpc':
/p2/var/php_gcov/PHP_4_4/ext/standard/image.c:604: warning: unused variable `dummy_int'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/standard/image.c: In function `php_handle_jpc':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/standard/image.c
            [3] => 
            [4] => php_handle_jpc
            [5] => /p2/var/php_gcov/PHP_4_4/ext/standard/image.c
            [6] => 604
            [7] => warning
            [8] => unused variable `dummy_int'
        )

    [12] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c: In function `php_gd_parse':
/p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c:1138: warning: implicit declaration of function `php_gd_lex'
            [1] => /p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c: In function `php_gd_parse':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c
            [3] => 
            [4] => php_gd_parse
            [5] => /p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.c
            [6] => 1138
            [7] => warning
            [8] => implicit declaration of function `php_gd_lex'
        )

    [13] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.y:864: warning: return type defaults to `int'
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => /p2/var/php_gcov/PHP_4_4/ext/standard/parsedate.y
            [6] => 864
            [7] => warning
            [8] => return type defaults to `int'
        )

    [14] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c: In function `zif_msg_receive':
/p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c:318: warning: passing arg 2 of `php_var_unserialize' from incompatible pointer type
            [1] => /p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c: In function `zif_msg_receive':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c
            [3] => 
            [4] => zif_msg_receive
            [5] => /p2/var/php_gcov/PHP_4_4/ext/sysvmsg/sysvmsg.c
            [6] => 318
            [7] => warning
            [8] => passing arg 2 of `php_var_unserialize' from incompatible pointer type
        )

    [15] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/ext/yp/yp.c: In function `zif_yp_err_string':
/p2/var/php_gcov/PHP_4_4/ext/yp/yp.c:372: warning: assignment discards qualifiers from pointer target type
            [1] => /p2/var/php_gcov/PHP_4_4/ext/yp/yp.c: In function `zif_yp_err_string':

            [2] => /p2/var/php_gcov/PHP_4_4/ext/yp/yp.c
            [3] => 
            [4] => zif_yp_err_string
            [5] => /p2/var/php_gcov/PHP_4_4/ext/yp/yp.c
            [6] => 372
            [7] => warning
            [8] => assignment discards qualifiers from pointer target type
        )

    [16] => Array
        (
            [0] => Zend/zend_language_scanner.c:5944: warning: `yy_fatal_error' defined but not used
Zend/zend_language_scanner.c:2627: warning: `yy_last_accepting_state' defined but not used
Zend/zend_language_scanner.c:2628: warning: `yy_last_accepting_cpos' defined but not used
Zend/zend_language_scanner.c:2634: warning: `yy_more_flag' defined but not used
Zend/zend_language_scanner.c:2635: warning: `yy_more_len' defined but not used
Zend/zend_language_scanner.c:5483: warning: `yyunput' defined but not used
Zend/zend_language_scanner.c:5929: warning: `yy_top_state' defined but not used
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => Zend/zend_language_scanner.c
            [6] => 5944
            [7] => warning
            [8] => `yy_fatal_error' defined but not used
            [9] => 2627
            [10] => warning
            [11] => `yy_last_accepting_state' defined but not used
            [12] => 2628
            [13] => warning
            [14] => `yy_last_accepting_cpos' defined but not used
            [15] => 2634
            [16] => warning
            [17] => `yy_more_flag' defined but not used
            [18] => 2635
            [19] => warning
            [20] => `yy_more_len' defined but not used
            [21] => 5483
            [22] => warning
            [23] => `yyunput' defined but not used
            [24] => 5929
            [25] => warning
            [26] => `yy_top_state' defined but not used
        )

    [17] => Array
        (
            [0] => Zend/zend_ini_scanner.c:457: warning: `yy_last_accepting_state' defined but not used
Zend/zend_ini_scanner.c:458: warning: `yy_last_accepting_cpos' defined but not used
Zend/zend_ini_scanner.c:1361: warning: `yyunput' defined but not used
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => Zend/zend_ini_scanner.c
            [6] => 457
            [7] => warning
            [8] => `yy_last_accepting_state' defined but not used
            [9] => 458
            [10] => warning
            [11] => `yy_last_accepting_cpos' defined but not used
            [12] => 1361
            [13] => warning
            [14] => `yyunput' defined but not used
        )

    [18] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c: In function `_safe_emalloc':
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c:237: warning: long int format, size_t arg (arg 3)
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c:237: warning: long int format, size_t arg (arg 4)
/p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c:237: warning: long int format, size_t arg (arg 5)
            [1] => /p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c: In function `_safe_emalloc':

            [2] => /p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c
            [3] => 
            [4] => _safe_emalloc
            [5] => /p2/var/php_gcov/PHP_4_4/Zend/zend_alloc.c
            [6] => 237
            [7] => warning
            [8] => long int format, size_t arg (arg 3)
            [9] => 237
            [10] => warning
            [11] => long int format, size_t arg (arg 4)
            [12] => 237
            [13] => warning
            [14] => long int format, size_t arg (arg 5)
        )

    [19] => Array
        (
            [0] => /p2/var/php_gcov/PHP_4_4/Zend/zend_ini.c:338: warning: `zend_ini_displayer_cb' defined but not used
            [1] => 
            [2] => 
            [3] => 
            [4] => 
            [5] => /p2/var/php_gcov/PHP_4_4/Zend/zend_ini.c
            [6] => 338
            [7] => warning
            [8] => `zend_ini_displayer_cb' defined but not used
        )

    [20] => Array
        (
            [0] => ext/mysql/libmysql/my_tempnam.o(.text+0x80): In function `my_tempnam':
/p2/var/php_gcov/PHP_4_4/ext/mysql/libmysql/my_tempnam.c:115: warning: the use of `tempnam' is dangerous, better use `mkstemp'
            [1] => ext/mysql/libmysql/my_tempnam.o(.text+0x80): In function `my_tempnam':

            [2] => ext/mysql/libmysql/my_tempnam.o
            [3] => (.text+0x80)
            [4] => my_tempnam
            [5] => /p2/var/php_gcov/PHP_4_4/ext/mysql/libmysql/my_tempnam.c
            [6] => 115
            [7] => warning
            [8] => the use of `tempnam' is dangerous, better use `mkstemp'
        )

    [21] => Array
        (
            [0] => ext/mysql/libmysql/my_tempnam.o(.text+0x80): In function `my_tempnam':
/p2/var/php_gcov/PHP_4_4/ext/mysql/libmysql/my_tempnam.c:115: warning: the use of `tempnam' is dangerous, better use `mkstemp'
            [1] => ext/mysql/libmysql/my_tempnam.o(.text+0x80): In function `my_tempnam':

            [2] => ext/mysql/libmysql/my_tempnam.o
            [3] => (.text+0x80)
            [4] => my_tempnam
            [5] => /p2/var/php_gcov/PHP_4_4/ext/mysql/libmysql/my_tempnam.c
            [6] => 115
            [7] => warning
            [8] => the use of `tempnam' is dangerous, better use `mkstemp'
        )

    [22] => Array
        (
            [0] => ext/ming/ming.o(.text+0xc115): In function `zim_swfmovie_namedAnchor':
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:2207: undefined reference to `SWFMovie_namedAnchor'
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:2209: undefined reference to `SWFMovie_xpto'
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:2259: undefined reference to `SWFMovie_foo'
            [1] => ext/ming/ming.o(.text+0xc115): In function `zim_swfmovie_namedAnchor':

            [2] => ext/ming/ming.o
            [3] => (.text+0xc115)
            [4] => zim_swfmovie_namedAnchor
            [5] => /p2/var/php_gcov/PHP_5_2/ext/ming/ming.c
            [6] => 2207
            [7] => 
            [8] => undefined reference to `SWFMovie_namedAnchor'
            [9] => 2209
            [10] => 
            [11] => undefined reference to `SWFMovie_xpto'
            [12] => 2259
            [13] => 
            [14] => undefined reference to `SWFMovie_foo'
        )

    [23] => Array
        (
            [0] => ext/ming/ming.o(.text+0x851): In function `zif_ming_setSWFCompression':
/p2/var/php_gcov/PHP_5_2/ext/ming/ming.c:154: undefined reference to `Ming_setSWFCompression'
            [1] => ext/ming/ming.o(.text+0x851): In function `zif_ming_setSWFCompression':

            [2] => ext/ming/ming.o
            [3] => (.text+0x851)
            [4] => zif_ming_setSWFCompression
            [5] => /p2/var/php_gcov/PHP_5_2/ext/ming/ming.c
            [6] => 154
            [7] => 
            [8] => undefined reference to `Ming_setSWFCompression'
        )

)
