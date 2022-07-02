--TEST--
Test semi-reserved words as class methods
--FILE--
<?php

class Obj
{
    function empty(){ echo __METHOD__, PHP_EOL; }
    function callable(){ echo __METHOD__, PHP_EOL; }
    function class(){ echo __METHOD__, PHP_EOL; }
    function trait(){ echo __METHOD__, PHP_EOL; }
    function extends(){ echo __METHOD__, PHP_EOL; }
    function implements(){ echo __METHOD__, PHP_EOL; }
    function static(){ echo __METHOD__, PHP_EOL; }
    function abstract(){ echo __METHOD__, PHP_EOL; }
    function final(){ echo __METHOD__, PHP_EOL; }
    function public(){ echo __METHOD__, PHP_EOL; }
    function protected(){ echo __METHOD__, PHP_EOL; }
    function private(){ echo __METHOD__, PHP_EOL; }
    function const(){ echo __METHOD__, PHP_EOL; }
    function enddeclare(){ echo __METHOD__, PHP_EOL; }
    function endfor(){ echo __METHOD__, PHP_EOL; }
    function endforeach(){ echo __METHOD__, PHP_EOL; }
    function endif(){ echo __METHOD__, PHP_EOL; }
    function endwhile(){ echo __METHOD__, PHP_EOL; }
    function and(){ echo __METHOD__, PHP_EOL; }
    function global(){ echo __METHOD__, PHP_EOL; }
    function goto(){ echo __METHOD__, PHP_EOL; }
    function instanceof(){ echo __METHOD__, PHP_EOL; }
    function insteadof(){ echo __METHOD__, PHP_EOL; }
    function interface(){ echo __METHOD__, PHP_EOL; }
    function namespace(){ echo __METHOD__, PHP_EOL; }
    function new(){ echo __METHOD__, PHP_EOL; }
    function or(){ echo __METHOD__, PHP_EOL; }
    function xor(){ echo __METHOD__, PHP_EOL; }
    function try(){ echo __METHOD__, PHP_EOL; }
    function use(){ echo __METHOD__, PHP_EOL; }
    function var(){ echo __METHOD__, PHP_EOL; }
    function exit(){ echo __METHOD__, PHP_EOL; }
    function list(){ echo __METHOD__, PHP_EOL; }
    function clone(){ echo __METHOD__, PHP_EOL; }
    function include(){ echo __METHOD__, PHP_EOL; }
    function include_once(){ echo __METHOD__, PHP_EOL; }
    function throw(){ echo __METHOD__, PHP_EOL; }
    function array(){ echo __METHOD__, PHP_EOL; }
    function print(){ echo __METHOD__, PHP_EOL; }
    function echo(){ echo __METHOD__, PHP_EOL; }
    function readonly(){ echo __METHOD__, PHP_EOL; }
    function require(){ echo __METHOD__, PHP_EOL; }
    function require_once(){ echo __METHOD__, PHP_EOL; }
    function return(){ echo __METHOD__, PHP_EOL; }
    function else(){ echo __METHOD__, PHP_EOL; }
    function elseif(){ echo __METHOD__, PHP_EOL; }
    function default(){ echo __METHOD__, PHP_EOL; }
    function break(){ echo __METHOD__, PHP_EOL; }
    function continue(){ echo __METHOD__, PHP_EOL; }
    function switch(){ echo __METHOD__, PHP_EOL; }
    function yield(){ echo __METHOD__, PHP_EOL; }
    function function(){ echo __METHOD__, PHP_EOL; }
    function fn(){ echo __METHOD__, PHP_EOL; }
    function if(){ echo __METHOD__, PHP_EOL; }
    function endswitch(){ echo __METHOD__, PHP_EOL; }
    function finally(){ echo __METHOD__, PHP_EOL; }
    function for(){ echo __METHOD__, PHP_EOL; }
    function foreach(){ echo __METHOD__, PHP_EOL; }
    function declare(){ echo __METHOD__, PHP_EOL; }
    function case(){ echo __METHOD__, PHP_EOL; }
    function do(){ echo __METHOD__, PHP_EOL; }
    function while(){ echo __METHOD__, PHP_EOL; }
    function as(){ echo __METHOD__, PHP_EOL; }
    function catch(){ echo __METHOD__, PHP_EOL; }
    function die(){ echo __METHOD__, PHP_EOL; }
    function self(){ echo __METHOD__, PHP_EOL; }
    function parent(){ echo __METHOD__, PHP_EOL; }
    function isset(){ echo __METHOD__, PHP_EOL; }
    function unset(){ echo __METHOD__, PHP_EOL; }
    function __CLASS__(){ echo __METHOD__, PHP_EOL; }
    function __TRAIT__(){ echo __METHOD__, PHP_EOL; }
    function __FUNCTION__(){ echo __METHOD__, PHP_EOL; }
    function __METHOD__(){ echo __METHOD__, PHP_EOL; }
    function __LINE__(){ echo __METHOD__, PHP_EOL; }
    function __FILE__(){ echo __METHOD__, PHP_EOL; }
    function __DIR__(){ echo __METHOD__, PHP_EOL; }
    function __NAMESPACE__(){ echo __METHOD__, PHP_EOL; }
}

$obj = new Obj;

$obj->empty();
$obj->callable();
$obj->class();
$obj->trait();
$obj->extends();
$obj->implements();
$obj->static();
$obj->abstract();
$obj->final();
$obj->public();
$obj->protected();
$obj->private();
$obj->const();
$obj->enddeclare();
$obj->endfor();
$obj->endforeach();
$obj->endif();
$obj->endwhile();
$obj->and();
$obj->global();
$obj->goto();
$obj->instanceof();
$obj->insteadof();
$obj->interface();
$obj->namespace();
$obj->new();
$obj->or();
$obj->xor();
$obj->try();
$obj->use();
$obj->var();
$obj->exit();
$obj->list();
$obj->clone();
$obj->include();
$obj->include_once();
$obj->throw();
$obj->array();
$obj->print();
$obj->echo();
$obj->readonly();
$obj->require();
$obj->require_once();
$obj->return();
$obj->else();
$obj->elseif();
$obj->default();
$obj->break();
$obj->continue();
$obj->switch();
$obj->yield();
$obj->function();
$obj->fn();
$obj->if();
$obj->endswitch();
$obj->finally();
$obj->for();
$obj->foreach();
$obj->declare();
$obj->case();
$obj->do();
$obj->while();
$obj->as();
$obj->catch();
$obj->die();
$obj->self();
$obj->parent();
$obj->isset();
$obj->unset();
$obj->__CLASS__();
$obj->__TRAIT__();
$obj->__FUNCTION__();
$obj->__METHOD__();
$obj->__LINE__();
$obj->__FILE__();
$obj->__DIR__();
$obj->__NAMESPACE__();

echo "\nDone\n";
?>
--EXPECT--
Obj::empty
Obj::callable
Obj::class
Obj::trait
Obj::extends
Obj::implements
Obj::static
Obj::abstract
Obj::final
Obj::public
Obj::protected
Obj::private
Obj::const
Obj::enddeclare
Obj::endfor
Obj::endforeach
Obj::endif
Obj::endwhile
Obj::and
Obj::global
Obj::goto
Obj::instanceof
Obj::insteadof
Obj::interface
Obj::namespace
Obj::new
Obj::or
Obj::xor
Obj::try
Obj::use
Obj::var
Obj::exit
Obj::list
Obj::clone
Obj::include
Obj::include_once
Obj::throw
Obj::array
Obj::print
Obj::echo
Obj::readonly
Obj::require
Obj::require_once
Obj::return
Obj::else
Obj::elseif
Obj::default
Obj::break
Obj::continue
Obj::switch
Obj::yield
Obj::function
Obj::fn
Obj::if
Obj::endswitch
Obj::finally
Obj::for
Obj::foreach
Obj::declare
Obj::case
Obj::do
Obj::while
Obj::as
Obj::catch
Obj::die
Obj::self
Obj::parent
Obj::isset
Obj::unset
Obj::__CLASS__
Obj::__TRAIT__
Obj::__FUNCTION__
Obj::__METHOD__
Obj::__LINE__
Obj::__FILE__
Obj::__DIR__
Obj::__NAMESPACE__

Done
