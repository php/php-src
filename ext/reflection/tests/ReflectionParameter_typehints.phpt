--TEST--
ReflectionParameter class - Test is*() functions for typehints
--FILE--
<?php

$functions = [
    'callable $x' => function (callable $x) {},
    'array $x' => function (array $x) {},
    'int $x' => function (int $x) {},
    'float $x' => function (float $x) {},
    'string $x' => function (string $x) {},
    'bool $x' => function (bool $x) {},
    'resource $x' => function (resource $x) {}
];
$methods = [
    'isCallable',
    'isArray',
    'isInt',
    'isFloat',
    'isString',
    'isBool',
    'isResource'
];

$colwidth = 14;

echo str_pad("", $colwidth);
foreach ($methods as $method) {
    echo str_pad($method, $colwidth);
}
echo "\n";

foreach ($functions as $name => $function) {
    echo str_pad($name, $colwidth);
    $reflection = new ReflectionFunction($function);
    $param = $reflection->getParameters()[0];
    foreach ($methods as $method) {
        echo str_pad($param->{$method}(), $colwidth);
    }
    echo "\n";
}

?>
--EXPECTF--
              isCallable    isArray       isInt         isFloat       isString      isBool        isResource    
callable $x   1                                                                                                 
array $x                    1                                                                                   
int $x                                    1                                                                     
float $x                                                1                                                       
string $x                                                             1                                         
bool $x                                                                             1                           
resource $x                                                                                       1             