--TEST--
GH-21824 (Crash in unserialize() with destructor reentry)
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php
class Trigger {
    public function __destruct() {
        @unserialize('i:1;');
    }
}

$payload = 'a:1019:{';
for ($i = 0; $i < 1017; $i++) {
    $payload .= "i:$i;s:1:\"A\";";
}
$payload .= 'i:1017;O:7:"Trigger":0:{}';
$payload .= 'i:1017;i:0;}';

@unserialize($payload);
echo "Done\n";
?>
--EXPECT--
Done
