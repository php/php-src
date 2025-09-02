--TEST--
Bug GH-8847: PHP hanging infinitly at 100% cpu when check php syntaxe of a valid file
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
class klass
{
    public function method(int $int, bool $bool=false) : int
    {
        while($int >= 0 && !$function1)
            $int--;
        if($bool)
            while($int >= 0 && $function1)
                $int--;
        return $int;
    }
}
?>
DONE
--EXPECTF--
DONE
