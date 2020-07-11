--TEST--
JIT ASSIGN: Assign with INTERNED string(no RC)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class A {
    public $result = "string";
    function __set($propName, $propValue)
    {
        $oldType = \gettype($this->$propName);
        $newType = \gettype($propValue);
        if ($propValue === 'false')
        {
            $newType   = 'boolean';
            $propValue = \false;
        }
        elseif ($propValue === 'true')
        {
            $newType   = 'boolean';
            $propValue = \true;
        }
        if ($oldType !== $newType)
        {
            $tmp = $propValue;
            \settype($tmp, $newType);
        }
        $this->propName = $propValue;
    }
}
$a = new A;
$a->result = "okey";
echo $a->result;
?>
--EXPECT--
okey
