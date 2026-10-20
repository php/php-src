--TEST--
GH-22683 (Reflection(Class)Constant::__toString() should not warn on NAN conversions)
--FILE--
<?php

echo new ReflectionConstant('NAN');
echo new ReflectionConstant('INF');

class Demo {
    public const MY_NAN = NAN;
    public const MY_INF = INF;
    public const MY_FLOAT = 1.5;
    public const MY_WHOLE = 2.0;
}

echo new ReflectionClassConstant(Demo::class, 'MY_NAN');
echo new ReflectionClassConstant(Demo::class, 'MY_INF');
echo new ReflectionClassConstant(Demo::class, 'MY_FLOAT');
echo new ReflectionClassConstant(Demo::class, 'MY_WHOLE');

?>
--EXPECT--
Constant [ <persistent> float NAN ] { NAN }
Constant [ <persistent> float INF ] { INF }
Constant [ public float MY_NAN ] { NAN }
Constant [ public float MY_INF ] { INF }
Constant [ public float MY_FLOAT ] { 1.5 }
Constant [ public float MY_WHOLE ] { 2 }
