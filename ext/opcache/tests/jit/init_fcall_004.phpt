--TEST--
JIT INIT_FCALL: 004 Invalid target opline with jit->reuse_ip active
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php

function gen() {
    yield 1;
    yield 2;
    return 3;
};

opcache_jit_blacklist(gen(...));

for ($i = 0; $i < 2; ++$i) {
    foreach (gen() as $val) {
        var_dump($val);
    }
}
?>
DONE
--EXPECT--
int(1)
int(2)
int(1)
int(2)
DONE
