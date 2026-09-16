--TEST--
serialize(): a by-reference __serialize() that grows the array being walked must not free it
--FILE--
<?php
class G {
    public $ref;
    public function __serialize(): array {
        for ($i = 0; $i < 128; $i++) {
            $this->ref[] = 'x' . $i;
        }
        return ['d' => 1];
    }
}
$g = new G();
$inner = [$g, 'tail'];
$g->ref = &$inner;
$top = [&$inner];
var_dump(serialize($top));
?>
--EXPECT--
string(59) "a:1:{i:0;a:2:{i:0;O:1:"G":1:{s:1:"d";i:1;}i:1;s:4:"tail";}}"
