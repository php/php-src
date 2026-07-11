--TEST--
GH-21824 (Crash in unserialize() when a destructor reentrantly reuses the context)
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
// Overflow into a second var_entries block, then a duplicate key pushes Trigger
// onto the delayed-destructor list.
$payload .= 'i:1017;O:7:"Trigger":0:{}';
$payload .= 'i:1017;i:0;}';
$result = @unserialize($payload);
var_dump(is_array($result), count($result));
echo "done\n";
?>
--EXPECT--
bool(true)
int(1018)
done
