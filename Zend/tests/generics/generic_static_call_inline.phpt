--TEST--
Generic static method call inlined by optimizer (no typed params, returns constant)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFEBFFF
--EXTENSIONS--
opcache
--FILE--
<?php

class Box<T> {
    public static function hello(): string {
        return "Hello from Box!";
    }

    public static function getClass(): string {
        return self::class;
    }
}

// These calls are eligible for optimizer inlining (pass 4+16):
// no typed params + body is just "return constant"
echo Box<int>::hello() . "\n";
echo Box<string>::hello() . "\n";
echo Box<int>::getClass() . "\n";

echo "OK\n";
?>
--EXPECT--
Hello from Box!
Hello from Box!
Box
OK
