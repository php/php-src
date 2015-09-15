--TEST--
Test semi-reserved words as class properties
--FILE--
<?php

class Obj
{
    var $empty = 'empty';
    var $callable = 'callable';
    var $class = 'class';
    var $trait = 'trait';
    var $extends = 'extends';
    var $implements = 'implements';
    var $static = 'static';
    var $abstract = 'abstract';
    var $final = 'final';
    var $public = 'public';
    var $protected = 'protected';
    var $private = 'private';
    var $const = 'const';
    var $enddeclare = 'enddeclare';
    var $endfor = 'endfor';
    var $endforeach = 'endforeach';
    var $endif = 'endif';
    var $endwhile = 'endwhile';
    var $and = 'and';
    var $global = 'global';
    var $goto = 'goto';
    var $instanceof = 'instanceof';
    var $insteadof = 'insteadof';
    var $interface = 'interface';
    var $namespace = 'namespace';
    var $new = 'new';
    var $or = 'or';
    var $xor = 'xor';
    var $try = 'try';
    var $use = 'use';
    var $var = 'var';
    var $exit = 'exit';
    var $list = 'list';
    var $clone = 'clone';
    var $include = 'include';
    var $include_once = 'include_once';
    var $throw = 'throw';
    var $array = 'array';
    var $print = 'print';
    var $echo = 'echo';
    var $require = 'require';
    var $require_once = 'require_once';
    var $return = 'return';
    var $else = 'else';
    var $elseif = 'elseif';
    var $default = 'default';
    var $break = 'break';
    var $continue = 'continue';
    var $switch = 'switch';
    var $yield = 'yield';
    var $function = 'function';
    var $if = 'if';
    var $endswitch = 'endswitch';
    var $finally = 'finally';
    var $for = 'for';
    var $foreach = 'foreach';
    var $declare = 'declare';
    var $case = 'case';
    var $do = 'do';
    var $while = 'while';
    var $as = 'as';
    var $catch = 'catch';
    var $die = 'die';
    var $self = 'self';
    var $parent = 'parent';
    var $isset = 'isset';
    var $unset = 'unset';
    var $__CLASS__ = '__CLASS__';
    var $__TRAIT__ = '__TRAIT__';
    var $__FUNCTION__ = '__FUNCTION__';
    var $__METHOD__ = '__METHOD__';
    var $__LINE__ = '__LINE__';
    var $__FILE__ = '__FILE__';
    var $__DIR__ = '__DIR__';
    var $__NAMESPACE__ = '__NAMESPACE__';
    var $__halt_compiler = '__halt_compiler';
}

$obj = new Obj;

echo $obj->empty, PHP_EOL;
echo $obj->callable, PHP_EOL;
echo $obj->class, PHP_EOL;
echo $obj->trait, PHP_EOL;
echo $obj->extends, PHP_EOL;
echo $obj->implements, PHP_EOL;
echo $obj->static, PHP_EOL;
echo $obj->abstract, PHP_EOL;
echo $obj->final, PHP_EOL;
echo $obj->public, PHP_EOL;
echo $obj->protected, PHP_EOL;
echo $obj->private, PHP_EOL;
echo $obj->const, PHP_EOL;
echo $obj->enddeclare, PHP_EOL;
echo $obj->endfor, PHP_EOL;
echo $obj->endforeach, PHP_EOL;
echo $obj->endif, PHP_EOL;
echo $obj->endwhile, PHP_EOL;
echo $obj->and, PHP_EOL;
echo $obj->global, PHP_EOL;
echo $obj->goto, PHP_EOL;
echo $obj->instanceof, PHP_EOL;
echo $obj->insteadof, PHP_EOL;
echo $obj->interface, PHP_EOL;
echo $obj->namespace, PHP_EOL;
echo $obj->new, PHP_EOL;
echo $obj->or, PHP_EOL;
echo $obj->xor, PHP_EOL;
echo $obj->try, PHP_EOL;
echo $obj->use, PHP_EOL;
echo $obj->var, PHP_EOL;
echo $obj->exit, PHP_EOL;
echo $obj->list, PHP_EOL;
echo $obj->clone, PHP_EOL;
echo $obj->include, PHP_EOL;
echo $obj->include_once, PHP_EOL;
echo $obj->throw, PHP_EOL;
echo $obj->array, PHP_EOL;
echo $obj->print, PHP_EOL;
echo $obj->echo, PHP_EOL;
echo $obj->require, PHP_EOL;
echo $obj->require_once, PHP_EOL;
echo $obj->return, PHP_EOL;
echo $obj->else, PHP_EOL;
echo $obj->elseif, PHP_EOL;
echo $obj->default, PHP_EOL;
echo $obj->break, PHP_EOL;
echo $obj->continue, PHP_EOL;
echo $obj->switch, PHP_EOL;
echo $obj->yield, PHP_EOL;
echo $obj->function, PHP_EOL;
echo $obj->if, PHP_EOL;
echo $obj->endswitch, PHP_EOL;
echo $obj->finally, PHP_EOL;
echo $obj->for, PHP_EOL;
echo $obj->foreach, PHP_EOL;
echo $obj->declare, PHP_EOL;
echo $obj->case, PHP_EOL;
echo $obj->do, PHP_EOL;
echo $obj->while, PHP_EOL;
echo $obj->as, PHP_EOL;
echo $obj->catch, PHP_EOL;
echo $obj->die, PHP_EOL;
echo $obj->self, PHP_EOL;
echo $obj->parent, PHP_EOL;
echo $obj->isset, PHP_EOL;
echo $obj->unset, PHP_EOL;
echo $obj->__CLASS__, PHP_EOL;
echo $obj->__TRAIT__, PHP_EOL;
echo $obj->__FUNCTION__, PHP_EOL;
echo $obj->__METHOD__, PHP_EOL;
echo $obj->__LINE__, PHP_EOL;
echo $obj->__FILE__, PHP_EOL;
echo $obj->__DIR__, PHP_EOL;
echo $obj->__NAMESPACE__, PHP_EOL;
echo $obj->__halt_compiler, PHP_EOL;

echo "\nDone\n";

?>
--EXPECTF--
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
