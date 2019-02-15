--TEST--
Test semi-reserved words as static class properties
--FILE--
<?php

class Obj
{
    static $empty = 'empty';
    static $callable = 'callable';
    static $class = 'class';
    static $trait = 'trait';
    static $extends = 'extends';
    static $implements = 'implements';
    static $static = 'static';
    static $abstract = 'abstract';
    static $final = 'final';
    static $public = 'public';
    static $protected = 'protected';
    static $private = 'private';
    static $const = 'const';
    static $enddeclare = 'enddeclare';
    static $endfor = 'endfor';
    static $endforeach = 'endforeach';
    static $endif = 'endif';
    static $endwhile = 'endwhile';
    static $and = 'and';
    static $global = 'global';
    static $goto = 'goto';
    static $instanceof = 'instanceof';
    static $insteadof = 'insteadof';
    static $interface = 'interface';
    static $namespace = 'namespace';
    static $new = 'new';
    static $or = 'or';
    static $xor = 'xor';
    static $try = 'try';
    static $use = 'use';
    static $var = 'var';
    static $exit = 'exit';
    static $list = 'list';
    static $clone = 'clone';
    static $include = 'include';
    static $include_once = 'include_once';
    static $throw = 'throw';
    static $array = 'array';
    static $print = 'print';
    static $echo = 'echo';
    static $require = 'require';
    static $require_once = 'require_once';
    static $return = 'return';
    static $else = 'else';
    static $elseif = 'elseif';
    static $default = 'default';
    static $break = 'break';
    static $continue = 'continue';
    static $switch = 'switch';
    static $yield = 'yield';
    static $function = 'function';
    static $if = 'if';
    static $endswitch = 'endswitch';
    static $finally = 'finally';
    static $for = 'for';
    static $foreach = 'foreach';
    static $declare = 'declare';
    static $case = 'case';
    static $do = 'do';
    static $while = 'while';
    static $as = 'as';
    static $catch = 'catch';
    static $die = 'die';
    static $self = 'self';
    static $parent = 'parent';
    static $isset = 'isset';
    static $unset = 'unset';
    static $__CLASS__ = '__CLASS__';
    static $__TRAIT__ = '__TRAIT__';
    static $__FUNCTION__ = '__FUNCTION__';
    static $__METHOD__ = '__METHOD__';
    static $__LINE__ = '__LINE__';
    static $__FILE__ = '__FILE__';
    static $__DIR__ = '__DIR__';
    static $__NAMESPACE__ = '__NAMESPACE__';
    static $__halt_compiler = '__halt_compiler';
}

echo Obj::$empty, PHP_EOL;
echo Obj::$callable, PHP_EOL;
echo Obj::$class, PHP_EOL;
echo Obj::$trait, PHP_EOL;
echo Obj::$extends, PHP_EOL;
echo Obj::$implements, PHP_EOL;
echo Obj::$static, PHP_EOL;
echo Obj::$abstract, PHP_EOL;
echo Obj::$final, PHP_EOL;
echo Obj::$public, PHP_EOL;
echo Obj::$protected, PHP_EOL;
echo Obj::$private, PHP_EOL;
echo Obj::$const, PHP_EOL;
echo Obj::$enddeclare, PHP_EOL;
echo Obj::$endfor, PHP_EOL;
echo Obj::$endforeach, PHP_EOL;
echo Obj::$endif, PHP_EOL;
echo Obj::$endwhile, PHP_EOL;
echo Obj::$and, PHP_EOL;
echo Obj::$global, PHP_EOL;
echo Obj::$goto, PHP_EOL;
echo Obj::$instanceof, PHP_EOL;
echo Obj::$insteadof, PHP_EOL;
echo Obj::$interface, PHP_EOL;
echo Obj::$namespace, PHP_EOL;
echo Obj::$new, PHP_EOL;
echo Obj::$or, PHP_EOL;
echo Obj::$xor, PHP_EOL;
echo Obj::$try, PHP_EOL;
echo Obj::$use, PHP_EOL;
echo Obj::$var, PHP_EOL;
echo Obj::$exit, PHP_EOL;
echo Obj::$list, PHP_EOL;
echo Obj::$clone, PHP_EOL;
echo Obj::$include, PHP_EOL;
echo Obj::$include_once, PHP_EOL;
echo Obj::$throw, PHP_EOL;
echo Obj::$array, PHP_EOL;
echo Obj::$print, PHP_EOL;
echo Obj::$echo, PHP_EOL;
echo Obj::$require, PHP_EOL;
echo Obj::$require_once, PHP_EOL;
echo Obj::$return, PHP_EOL;
echo Obj::$else, PHP_EOL;
echo Obj::$elseif, PHP_EOL;
echo Obj::$default, PHP_EOL;
echo Obj::$break, PHP_EOL;
echo Obj::$continue, PHP_EOL;
echo Obj::$switch, PHP_EOL;
echo Obj::$yield, PHP_EOL;
echo Obj::$function, PHP_EOL;
echo Obj::$if, PHP_EOL;
echo Obj::$endswitch, PHP_EOL;
echo Obj::$finally, PHP_EOL;
echo Obj::$for, PHP_EOL;
echo Obj::$foreach, PHP_EOL;
echo Obj::$declare, PHP_EOL;
echo Obj::$case, PHP_EOL;
echo Obj::$do, PHP_EOL;
echo Obj::$while, PHP_EOL;
echo Obj::$as, PHP_EOL;
echo Obj::$catch, PHP_EOL;
echo Obj::$die, PHP_EOL;
echo Obj::$self, PHP_EOL;
echo Obj::$parent, PHP_EOL;
echo Obj::$isset, PHP_EOL;
echo Obj::$unset, PHP_EOL;
echo Obj::$__CLASS__, PHP_EOL;
echo Obj::$__TRAIT__, PHP_EOL;
echo Obj::$__FUNCTION__, PHP_EOL;
echo Obj::$__METHOD__, PHP_EOL;
echo Obj::$__LINE__, PHP_EOL;
echo Obj::$__FILE__, PHP_EOL;
echo Obj::$__DIR__, PHP_EOL;
echo Obj::$__NAMESPACE__, PHP_EOL;
echo Obj::$__halt_compiler, PHP_EOL;

echo "\nDone\n";
--EXPECT--
empty
callable
class
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
__halt_compiler

Done
