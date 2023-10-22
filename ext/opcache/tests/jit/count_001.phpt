--TEST--
JIT COUNT: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=1
--EXTENSIONS--
opcache
--FILE--
<?php
class ThrowsInDestructor {
    public function __destruct() {
        throw new RuntimeException("In destructor");
    }
}
class C {
    public static function create_array(int $i): array {
        return array_fill(0, $i, new stdClass());
    }

    public static function foo() {
        $x = [self::create_array(5)];
        echo count(self::create_array(0)), "\n";
        echo count(self::create_array(1)), "\n";
        echo count($x[0]), "\n";
        $a = [];
        for ($i = 0; $i < 4; $i++) {
            $a[] = $i;
            echo count($a) . "\n";
        }
    }
    public static function count_ref(array &$ref): int {
        return count($ref);
    }

    public static function count_throws(): int {
        $result = count([new ThrowsInDestructor()]);
        echo "Unreachable\n";
        return $result;
    }
}
C::foo();
$x = ['x', 'y', 'z', 'a', new stdClass()];
echo C::count_ref($x), "\n";
for ($i = 0; $i < 5; $i++) {
    try {
        echo C::count_throws(), "\n";
    } catch (RuntimeException $e) {
        printf("Caught %s\n", $e->getMessage());
    }
}

?>
--EXPECT--
0
1
5
1
2
3
4
5
Caught In destructor
Caught In destructor
Caught In destructor
Caught In destructor
Caught In destructor