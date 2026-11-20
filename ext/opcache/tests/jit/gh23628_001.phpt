--TEST--
GH-23628 001: Tracing JIT reads undefined property slots of a lazy proxy
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
    public int $count = 0;
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
    public function coalesce(int $n): int {
        $sum = 0;
        for ($i = 0; $i < $n; $i++) {
            $sum += $this->map['start']['next'] ?? 100;
        }
        return $sum;
    }
    public function read(int $n): int {
        $sum = 0;
        for ($i = 0; $i < $n; $i++) {
            $sum += $this->map['start']['next'];
        }
        return $sum;
    }
    public function write(int $n): int {
        for ($i = 0; $i < $n; $i++) {
            $this->map['start']['next'] = $i;
            $this->count++;
        }
        return $this->map['start']['next'];
    }
}

$reflector = new ReflectionClass(Table::class);

$proxy = $reflector->newLazyProxy(fn () => new Table());
var_dump($proxy->parse(100));
$proxy = $reflector->newLazyProxy(fn () => new Table());
var_dump($proxy->coalesce(100));
$proxy = $reflector->newLazyProxy(fn () => new Table());
var_dump($proxy->read(100));
$proxy = $reflector->newLazyProxy(fn () => new Table());
var_dump($proxy->write(100));
var_dump($proxy->count);

$ghost = $reflector->newLazyGhost(function (Table $table) {});
var_dump($ghost->parse(100));
?>
--EXPECT--
int(100)
int(100)
int(100)
int(99)
int(100)
int(100)
