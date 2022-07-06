--TEST--
Relative print commands
--INI--
opcache.enable_cli=0
--PHPDBG--
b foo
r
p
p o
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at foo]
prompt> string(4) "test"
[Breakpoint #0 in foo() at %s:15, hits: 1]
>00015: 		var_dump(strrev($baz));
 00016: 	}
 00017: 
prompt> [Stack in foo() (8 ops)]
L14-16 foo() %s - %s + 8 ops
 L14   #0     RECV                    1                                         $baz                
 L15   #1     INIT_FCALL%s %d %s "var_dump"                               
 L15   #2     INIT_FCALL%s %d %s "strrev"                                 
 L15   #3     SEND_VAR                $baz                 1                                        
 L15   #4     DO_%cCALL                                                          @0                  
 L15   #5     SEND_VAR                @0                   1                                        
 L15   #6     DO_%cCALL                                                                              
 L16   #7     RETURN<-1>              null                                                          
prompt> [L15 %s INIT_FCALL%s %d %s "var_dump"                                %s]
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
