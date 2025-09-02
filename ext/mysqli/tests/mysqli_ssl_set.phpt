--TEST--
mysqli_ssl_set() - test is a stub!
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
die("skip test is a stub!");

require_once 'skipifconnectfailure.inc';
if (!function_exists('mysqli_ssl_set'))
    die("skip function not available");
?>
--FILE--
<?php
    require_once 'connect.inc';

    /*
    This function always returns TRUE value.

    $link = mysqli_init();
    if (NULL !== ($tmp = @mysqli_ssl_set(
        $link,
        'The path name to the key file.',
        'The path name to the certificate file.',
        'The path name to the certificate authority file.',
        'The pathname to a directory that contains trusted SSL CA certificates in PEM format.',
        'A list of allowable ciphers to use for SSL encryption.')))
        printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);


    If SSL setup is incorrect my_mysqli_real_connect()
    will return an error when you attempt to connect.

    ... and the above SSL setup should be always incorrect.

    if (false !== ($tmp = my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)))
        printf("[008] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
    */
    print "done!\n";
?>
--EXPECT--
done!
