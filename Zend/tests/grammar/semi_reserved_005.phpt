--TEST--
Test semi-reserved words as class constants
--FILE--
<?php

class Obj
{
    const EMPTY = 'empty';
    const CALLABLE = 'callable';
    const TRAIT = 'trait';
    const EXTENDS = 'extends';
    const IMPLEMENTS = 'implements';
    const STATIC = 'static';
    const ABSTRACT = 'abstract';
    const FINAL = 'final';
    const PUBLIC = 'public';
    const PROTECTED = 'protected';
    const PRIVATE = 'private';
    const CONST = 'const';
    const ENDDECLARE = 'enddeclare';
    const ENDFOR = 'endfor';
    const ENDFOREACH = 'endforeach';
    const ENDIF = 'endif';
    const ENDWHILE = 'endwhile';
    const AND = 'and';
    const GLOBAL = 'global';
    const GOTO = 'goto';
    const INSTANCEOF = 'instanceof';
    const INSTEADOF = 'insteadof';
    const INTERFACE = 'interface';
    const NAMESPACE = 'namespace';
    const NEW = 'new';
    const OR = 'or';
    const XOR = 'xor';
    const TRY = 'try';
    const USE = 'use';
    const VAR = 'var';
    const EXIT = 'exit';
    const LIST = 'list';
    const CLONE = 'clone';
    const INCLUDE = 'include';
    const INCLUDE_ONCE = 'include_once';
    const THROW = 'throw';
    const ARRAY = 'array';
    const PRINT = 'print';
    const ECHO = 'echo';
    const REQUIRE = 'require';
    const REQUIRE_ONCE = 'require_once';
    const RETURN = 'return';
    const ELSE = 'else';
    const ELSEIF = 'elseif';
    const DEFAULT = 'default';
    const BREAK = 'break';
    const CONTINUE = 'continue';
    const SWITCH = 'switch';
    const YIELD = 'yield';
    const FUNCTION = 'function';
    const IF = 'if';
    const ENDSWITCH = 'endswitch';
    const FINALLY = 'finally';
    const FOR = 'for';
    const FOREACH = 'foreach';
    const DECLARE = 'declare';
    const CASE = 'case';
    const DO = 'do';
    const WHILE = 'while';
    const AS = 'as';
    const CATCH = 'catch';
    const DIE = 'die';
    const SELF = 'self';
    const PARENT = 'parent';
    const ISSET = 'isset';
    const UNSET = 'unset';
    const __CLASS__ = '__CLASS__';
    const __TRAIT__ = '__TRAIT__';
    const __FUNCTION__ = '__FUNCTION__';
    const __METHOD__ = '__METHOD__';
    const __LINE__ = '__LINE__';
    const __FILE__ = '__FILE__';
    const __DIR__ = '__DIR__';
    const __NAMESPACE__ = '__NAMESPACE__';
}

echo Obj::EMPTY, PHP_EOL;
echo Obj::CALLABLE, PHP_EOL;
echo Obj::TRAIT, PHP_EOL;
echo Obj::EXTENDS, PHP_EOL;
echo Obj::IMPLEMENTS, PHP_EOL;
echo Obj::STATIC, PHP_EOL;
echo Obj::ABSTRACT, PHP_EOL;
echo Obj::FINAL, PHP_EOL;
echo Obj::PUBLIC, PHP_EOL;
echo Obj::PROTECTED, PHP_EOL;
echo Obj::PRIVATE, PHP_EOL;
echo Obj::CONST, PHP_EOL;
echo Obj::ENDDECLARE, PHP_EOL;
echo Obj::ENDFOR, PHP_EOL;
echo Obj::ENDFOREACH, PHP_EOL;
echo Obj::ENDIF, PHP_EOL;
echo Obj::ENDWHILE, PHP_EOL;
echo Obj::AND, PHP_EOL;
echo Obj::GLOBAL, PHP_EOL;
echo Obj::GOTO, PHP_EOL;
echo Obj::INSTANCEOF, PHP_EOL;
echo Obj::INSTEADOF, PHP_EOL;
echo Obj::INTERFACE, PHP_EOL;
echo Obj::NAMESPACE, PHP_EOL;
echo Obj::NEW, PHP_EOL;
echo Obj::OR, PHP_EOL;
echo Obj::XOR, PHP_EOL;
echo Obj::TRY, PHP_EOL;
echo Obj::USE, PHP_EOL;
echo Obj::VAR, PHP_EOL;
echo Obj::EXIT, PHP_EOL;
echo Obj::LIST, PHP_EOL;
echo Obj::CLONE, PHP_EOL;
echo Obj::INCLUDE, PHP_EOL;
echo Obj::INCLUDE_ONCE, PHP_EOL;
echo Obj::THROW, PHP_EOL;
echo Obj::ARRAY, PHP_EOL;
echo Obj::PRINT, PHP_EOL;
echo Obj::ECHO, PHP_EOL;
echo Obj::REQUIRE, PHP_EOL;
echo Obj::REQUIRE_ONCE, PHP_EOL;
echo Obj::RETURN, PHP_EOL;
echo Obj::ELSE, PHP_EOL;
echo Obj::ELSEIF, PHP_EOL;
echo Obj::DEFAULT, PHP_EOL;
echo Obj::BREAK, PHP_EOL;
echo Obj::CONTINUE, PHP_EOL;
echo Obj::SWITCH, PHP_EOL;
echo Obj::YIELD, PHP_EOL;
echo Obj::FUNCTION, PHP_EOL;
echo Obj::IF, PHP_EOL;
echo Obj::ENDSWITCH, PHP_EOL;
echo Obj::FINALLY, PHP_EOL;
echo Obj::FOR, PHP_EOL;
echo Obj::FOREACH, PHP_EOL;
echo Obj::DECLARE, PHP_EOL;
echo Obj::CASE, PHP_EOL;
echo Obj::DO, PHP_EOL;
echo Obj::WHILE, PHP_EOL;
echo Obj::AS, PHP_EOL;
echo Obj::CATCH, PHP_EOL;
echo Obj::DIE, PHP_EOL;
echo Obj::SELF, PHP_EOL;
echo Obj::PARENT, PHP_EOL;
echo Obj::ISSET, PHP_EOL;
echo Obj::UNSET, PHP_EOL;
echo Obj::__CLASS__, PHP_EOL;
echo Obj::__TRAIT__, PHP_EOL;
echo Obj::__FUNCTION__, PHP_EOL;
echo Obj::__METHOD__, PHP_EOL;
echo Obj::__LINE__, PHP_EOL;
echo Obj::__FILE__, PHP_EOL;
echo Obj::__DIR__, PHP_EOL;
echo Obj::__NAMESPACE__, PHP_EOL;

echo "\nDone\n";
--EXPECT--
empty
callable
trait
extends
implements
static
abstract
final
public
protected
private
const
enddeclare
endfor
endforeach
endif
endwhile
and
global
goto
instanceof
insteadof
interface
namespace
new
or
xor
try
use
var
exit
list
clone
include
include_once
throw
array
print
echo
require
require_once
return
else
elseif
default
break
continue
switch
yield
function
if
endswitch
finally
for
foreach
declare
case
do
while
as
catch
die
self
parent
isset
unset
__CLASS__
__TRAIT__
__FUNCTION__
__METHOD__
__LINE__
__FILE__
__DIR__
__NAMESPACE__

Done
