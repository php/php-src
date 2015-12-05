--TEST--
Basic print functionality
--INI--
opcache.enable_cli=0
--PHPDBG--
p foo
p class \Foo\bar
p
p e
q
--EXPECTF--
[Successful compilation of %s]
prompt> [User Function foo (8 ops)]
L14-16 foo() %s - %s + 8 ops
 L14   #0     RECV                    1                                         $baz                
 L15   #1     INIT_FCALL%s %d %s "var_dump"           ~3                  
 L15   #2     INIT_FCALL%s %d %s "strrev"             ~9                  
 L15   #3     SEND_VAR                $baz                 ~9                   @8                  
 L15   #4     DO_%cCALL                ~9                                        @0                  
 L15   #5     SEND_VAR                @0                   ~3                   @2                  
 L15   #6     DO_%cCALL                ~3                                                            
 L16   #7     RETURN                  null                                                          
prompt> [User Class: Foo\Bar (2 methods)]
L5-7 Foo\Bar::Foo() %s - %s + 5 ops
 L5    #0     RECV                    1                                         $bar                
 L6    #1     INIT_NS_FCALL_BY_NAME                        "Foo\\var_dump"      ~7                  
 L6    #2     SEND_VAR_EX             $bar                 ~7                   @6                  
 L6    #3     DO_FCALL                ~7                                                            
 L7    #4     RETURN                  null                                                          
L9-9 Foo\Bar::baz() %s - %s + 1 ops
 L9    #0     RETURN                  null                                                          
prompt> [Not Executing!]
prompt> [Context %s (11 ops)]
L1-19 {main}() %s - %s + 11 ops
 L4    #0     NOP                                                                                   
 L14   #1     NOP                                                                                   
 L18   #2     NEW                     "Foo\\Bar"           J4                   @1                  
 L18   #3     DO_FCALL                ~10                                                           
 L18   #4     INIT_METHOD_CALL        @1                   "Foo"                ~11                 
 L18   #5     SEND_VAL_EX             "test"               ~11                  @10                 
 L18   #6     DO_FCALL                ~11                                                           
 L19   #7     INIT_FCALL%s %d %s "foo"                ~6                  
 L19   #8     SEND_VAL                "test"               ~6                   @5                  
 L19   #9     DO_FCALL                ~6                                                            
 L19   #10    RETURN                  1                                                             
prompt> 
--FILE--
<?php

namespace Foo {
	class Bar {
		function Foo($bar) {
			var_dump($bar);
		}

		function baz() { }
	}
}

namespace {
	function foo($baz) {
		var_dump(strrev($baz));
	}

	(new \Foo\Bar)->Foo("test");
	foo("test");
}
