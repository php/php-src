--TEST--
FTP with bogus resource
--CREDITS--
Michael Paul da Rosa <michael [at] michaelpaul [dot] com [dot] br>
PHP TestFest Dublin 2017
--EXTENSIONS--
ftp
pcntl
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
ftp_login(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_pwd(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_cdup(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_chdir(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_exec(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_raw(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_mkdir(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_rmdir(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_chmod(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_alloc(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_nlist(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_rawlist(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_mlsd(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_systype(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_fget(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_nb_fget(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_pasv(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_get(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_nb_get(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_nb_continue(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_fput(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_nb_fput(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_put(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_append(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_nb_put(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_size(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_mdtm(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_rename(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_delete(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_site(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_close(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_set_option(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
ftp_get_option(): Argument #1 ($ftp) must be of type FTP\Connection, resource given
