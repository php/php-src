--TEST--
Bug #80745 (JIT produces Assert failure and UNKNOWN:0 var_dumps in code involving bitshifts)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php

final class Message
{
    public $qr = false;

    public $opcode = 0;

    public $aa = false;
}

echo "Starting...\n";

function headerToBinary(Message $message)
{
        $flags = 0;
        $flags = ($flags << 1) | ($message->qr ? 1 : 0);
        $flags = ($flags << 4) | $message->opcode;
        var_dump($flags);
        $flags = ($flags << 1) | ($message->aa ? 1 : 0);
}

headerToBinary(new Message());

echo "PROBLEM NOT REPRODUCED !\n";
?>
--EXPECT--
Starting...
int(0)
PROBLEM NOT REPRODUCED !

