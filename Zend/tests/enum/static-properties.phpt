--TEST--
Enum allows static properties
--FILE--
<?php

trait Logger {
    protected static bool $loggingEnabled;
}

enum Environment {
    case DEV;
    case STAGE;
    case PROD;

    use Logger;

    protected static Environment $current;

    public static function set(Environment $new): void {
        self::$current = $new;
        echo self::$current->name, "\n";
        self::$loggingEnabled = self::$current !== self::PROD;
        var_dump(self::$loggingEnabled);
    }
}

Environment::set(Environment::DEV);

?>
--EXPECT--
DEV
bool(true)
