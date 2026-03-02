--TEST--
Bug #73789 (Strange behavior of class constants in switch/case block)
--EXTENSIONS--
opcache
ctype
--FILE--
<?php
class Lexer
{
    const T_NONE = 1;
    const T_STRING = 2;
    const T_DOT = 8;
    public function getType($value): int
    {
        $type = self::T_NONE;
        switch (true) {
        case ctype_alpha($value[0]):
            $name = 'Lexer::T_' . strtoupper($value);
            $type = constant($name);
            if ($type > 100) {
                return $type;
            }
            return self::T_STRING;
        case $value === '.':
            return self::T_DOT;
        default:
        }
        return $type;
    }
}
var_dump((new Lexer())->getType("dot"));
?>
--EXPECT--
int(2)
