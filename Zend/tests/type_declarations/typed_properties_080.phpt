--TEST--
Access to typed static properties before initialization
--FILE--
<?php

class Test {
    public static int $a;
    protected static int $b;
    private static int $c;

    static function run() {
        try {
            self::$a;
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        try {
            self::$b;
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        try {
            self::$c;
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

Test::run();

?>
--EXPECT--
Error: Typed static property Test::$a must not be accessed before initialization
Error: Typed static property Test::$b must not be accessed before initialization
Error: Typed static property Test::$c must not be accessed before initialization
