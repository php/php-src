--TEST--
Bug GH-9164: Segfault in zend_accel_class_hash_copy
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable_cli=1
--FILE--
<?php

$incfile = __DIR__.'/gh9164.inc';

// Generate enough classes so that the out of bound access will cause a crash
// without relying on assertion
$fd = fopen($incfile, 'w');
fwrite($fd, "<?php\n");
for ($i = 0; $i < 4096; $i++) {
    fprintf($fd, "class FooBar%04d {}\n", $i);
}
fclose($fd);

// Ensure cacheability
touch($incfile, time() - 3600);

$pid = pcntl_fork();
if ($pid == 0) {
    // Child: Declare classes to allocate CE cache slots.
    require $incfile;
} else if ($pid > 0) {
    pcntl_wait($status);
    // Ensure that file has been cached. If not, this is testing nothing anymore.
    if (!isset(opcache_get_status()['scripts'][$incfile])) {
        print "File not cached\n";
    }
    // Populates local cache
    require $incfile;
    var_dump(new FooBar4095);
    unlink($incfile);
} else {
    echo "pcntl_fork() failed\n";
}

?>
--EXPECTF--
object(FooBar4095)#%d (0) {
}
