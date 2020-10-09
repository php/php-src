--TEST--
FTP with bogus resource
--CREDITS--
Michael Paul da Rosa <michael [at] michaelpaul [dot] com [dot] br>
PHP TestFest Dublin 2017
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$ftp = tmpfile();

try {
    var_dump(ftp_login($ftp, 'user', 'pass'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_pwd($ftp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_cdup($ftp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_chdir($ftp, '~'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_exec($ftp, 'x'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_raw($ftp, 'x'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_mkdir($ftp, '/'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_rmdir($ftp, '/'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_chmod($ftp, 7777, '/'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_alloc($ftp, 7777));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_nlist($ftp, '/'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_rawlist($ftp, '~'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_mlsd($ftp, '~'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_systype($ftp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_fget($ftp, $ftp, 'remote', 7777));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_nb_fget($ftp, $ftp, 'remote', 7777));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_pasv($ftp, false));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_get($ftp, 'local', 'remote', 7777));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_nb_get($ftp, 'local', 'remote', 7777));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_nb_continue($ftp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_fput($ftp, 'remote', $ftp, 9999));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_nb_fput($ftp, 'remote', $ftp, 9999));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_put($ftp, 'remote', 'local', 9999));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_append($ftp, 'remote', 'local', 9999));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_nb_put($ftp, 'remote', 'local', 9999));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_size($ftp, '~'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_mdtm($ftp, '~'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_rename($ftp, 'old', 'new'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_delete($ftp, 'gone'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_site($ftp, 'localhost'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_close($ftp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_set_option($ftp, 1, 2));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(ftp_get_option($ftp, 1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

fclose($ftp);
?>
--EXPECT--
ftp_login(): supplied resource is not a valid FTP Buffer resource
ftp_pwd(): supplied resource is not a valid FTP Buffer resource
ftp_cdup(): supplied resource is not a valid FTP Buffer resource
ftp_chdir(): supplied resource is not a valid FTP Buffer resource
ftp_exec(): supplied resource is not a valid FTP Buffer resource
ftp_raw(): supplied resource is not a valid FTP Buffer resource
ftp_mkdir(): supplied resource is not a valid FTP Buffer resource
ftp_rmdir(): supplied resource is not a valid FTP Buffer resource
ftp_chmod(): supplied resource is not a valid FTP Buffer resource
ftp_alloc(): supplied resource is not a valid FTP Buffer resource
ftp_nlist(): supplied resource is not a valid FTP Buffer resource
ftp_rawlist(): supplied resource is not a valid FTP Buffer resource
ftp_mlsd(): supplied resource is not a valid FTP Buffer resource
ftp_systype(): supplied resource is not a valid FTP Buffer resource
ftp_fget(): supplied resource is not a valid FTP Buffer resource
ftp_nb_fget(): supplied resource is not a valid FTP Buffer resource
ftp_pasv(): supplied resource is not a valid FTP Buffer resource
ftp_get(): supplied resource is not a valid FTP Buffer resource
ftp_nb_get(): supplied resource is not a valid FTP Buffer resource
ftp_nb_continue(): supplied resource is not a valid FTP Buffer resource
ftp_fput(): supplied resource is not a valid FTP Buffer resource
ftp_nb_fput(): supplied resource is not a valid FTP Buffer resource
ftp_put(): supplied resource is not a valid FTP Buffer resource
ftp_append(): supplied resource is not a valid FTP Buffer resource
ftp_nb_put(): supplied resource is not a valid FTP Buffer resource
ftp_size(): supplied resource is not a valid FTP Buffer resource
ftp_mdtm(): supplied resource is not a valid FTP Buffer resource
ftp_rename(): supplied resource is not a valid FTP Buffer resource
ftp_delete(): supplied resource is not a valid FTP Buffer resource
ftp_site(): supplied resource is not a valid FTP Buffer resource
ftp_close(): supplied resource is not a valid FTP Buffer resource
ftp_set_option(): supplied resource is not a valid FTP Buffer resource
ftp_get_option(): supplied resource is not a valid FTP Buffer resource
