--TEST--
ReflectionProperty::__construct(): ensure inherited private props can't be accessed through ReflectionProperty.
--FILE--
<?php

class C {
    private $p = 1;

    static function testFromC() {
        try {
          $rp = new ReflectionProperty("D", "p");
          var_dump($rp);
        } catch (Exception $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
}

class D extends C{
    static function testFromD() {
        try {
          $rp = new ReflectionProperty("D", "p");
          var_dump($rp);
        } catch (Exception $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
}

echo "--> Reflect inherited private from global scope:\n";
try {
  $rp = new ReflectionProperty("D", "p");
  var_dump($rp);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n--> Reflect inherited private from declaring scope:\n";
C::testFromC();

echo "\n\n--> Reflect inherited private from declaring scope via subclass:\n";
D::testFromC();

echo "\n\n--> Reflect inherited private from subclass:\n";
D::testFromD();
?>
--EXPECT--
--> Reflect inherited private from global scope:
ReflectionException: Property D::$p does not exist

--> Reflect inherited private from declaring scope:
ReflectionException: Property D::$p does not exist


--> Reflect inherited private from declaring scope via subclass:
ReflectionException: Property D::$p does not exist


--> Reflect inherited private from subclass:
ReflectionException: Property D::$p does not exist
