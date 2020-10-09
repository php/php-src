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
 L15   #1     INIT_FCALL%s %d %s "var_dump"                               
 L15   #2     INIT_FCALL%s %d %s "strrev"                                 
 L15   #3     SEND_VAR                $baz                 1                                        
 L15   #4     DO_%cCALL                                                          @0                  
 L15   #5     SEND_VAR                @0                   1                                        
 L15   #6     DO_%cCALL                                                                              
 L16   #7     RETURN<-1>              null                                                          
prompt> [User Class: Foo\Bar (2 methods)]
L5-7 Foo\Bar::Foo() %s - %s + 5 ops
 L5    #0     RECV                    1                                         $bar                
 L6    #1     INIT_NS_FCALL_BY_NAME<1>                      "Foo\\var_dump"                          
 L6    #2     SEND_VAR_EX             $bar                 1                                        
 L6    #3     DO_FCALL                                                                              
 L7    #4     RETURN<-1>              null                                                          
L9-9 Foo\Bar::baz() %s - %s + 1 ops
 L9    #0     RETURN<-1>              null                                                          
prompt> [Not Executing!]
prompt> [Context %s (9 ops)]
L1-21 {main}() %s - %s + 9 ops
 L18   #0     NEW                     "Foo\\Bar"                                @0                  
 L18   #1     DO_FCALL                                                                              
 L18   #2     INIT_METHOD_CALL<1>     @0                   "Foo"                                    
 L18   #3     SEND_VAL_EX             "test"               1                                        
 L18   #4     DO_FCALL                                                                              
 L19   #5     INIT_FCALL%s %d %s "foo"                                    
 L19   #6     SEND_VAL                "test"               1                                        
 L19   #7     DO_FCALL                                                                              
 L21   #8     RETURN<-1>              1                                                             
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
