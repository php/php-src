--TEST--
Register Alloction 001: Spilling in "identical" code
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
class Caster
{
    const EXCLUDE_PUBLIC = 8;
    const EXCLUDE_PRIVATE = 32;
    const EXCLUDE_STRICT = 512;

    public static function filter(array $a, $filter): int
    {
        foreach ($a as $k => $v) {
            if (!isset($k[1])) {
                $type |= self::EXCLUDE_PUBLIC;
            } else {
                $type |= self::EXCLUDE_PRIVATE;
            }

            if ((self::EXCLUDE_STRICT & $filter) ? $type === $filter : $type) {
            }
        }

        return $a;
    }

}
?>
OK
--EXPECT--
OK
