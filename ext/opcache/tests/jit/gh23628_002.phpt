--TEST--
GH-23628 002: Tracing JIT deoptimization on an undefined property slot of a lazy proxy
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=32M
opcache.jit_hot_loop=16
--EXTENSIONS--
opcache
--FILE--
<?php
final class Table {
    protected array $map = ['start' => ['next' => 1]];
    public function parse(int $n): int {
        $ok = 0;
        for ($i = 0; $i < $n; $i++) {
            if (isset($this->map['start']['next'])) {
                $ok++;
            } else {
                throw new RuntimeException('isset false at ' . $i);
            }
        }
        return $ok;
    }
}

// The trace is recorded and compiled for a regular object, so that the
// property is read directly from the property slot...
var_dump((new Table())->parse(100));

// ... and later executed for a lazy proxy, whose property slot is undefined
// and has to be forwarded to the real instance during deoptimization.
$proxy = (new ReflectionClass(Table::class))->newLazyProxy(fn () => new Table());
var_dump($proxy->parse(100));
?>
--EXPECT--
int(100)
int(100)
