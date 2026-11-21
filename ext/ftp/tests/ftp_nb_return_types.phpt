--TEST--
The non-blocking transfer functions declare the false their in_use guard returns
--EXTENSIONS--
ftp
--FILE--
<?php

/* ftp_nb_fget(), ftp_nb_get(), ftp_nb_fput() and ftp_nb_put() all answer an
 * already busy connection with false; ftp_nb_continue() has no such guard. */
foreach (['ftp_nb_fget', 'ftp_nb_get', 'ftp_nb_fput', 'ftp_nb_put', 'ftp_nb_continue'] as $function) {
    printf("%-17s %s\n", $function, (new ReflectionFunction($function))->getReturnType());
}

?>
--EXPECT--
ftp_nb_fget       int|false
ftp_nb_get        int|false
ftp_nb_fput       int|false
ftp_nb_put        int|false
ftp_nb_continue   int
