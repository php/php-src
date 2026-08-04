--TEST--
GH-22047: ArrayObject invalid iterator class in serialized payload
--CREDITS--
Igor Sak-Sakovskiy (Positive Technologies)
--FILE--
<?php

$payload = 'O:11:"ArrayObject":4:{i:0;i:0;i:1;a:2:{i:4;d:0.0;i:1;b:1;}i:2;a:0:{}i:3;s:12:"GlobIterator";}';

try {
    $obj = unserialize($payload);
    foreach ($obj as $k => $v) {
        echo "should not reach here\n";
    }
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Cannot deserialize ArrayObject with iterator class 'GlobIterator'; this class is not derived from ArrayIterator
