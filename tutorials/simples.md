Simples
=======
*Everything is simple ...*

It is easy to imagine that debugging a whole web application might be difficult, in a console program, in fact this is not the case:
 
A web application is just an instance of the PHP interpreter with some very specific super globals set.

With everything in the correct place, your web application has no clue that it being served not to a client, but to you, probably in your underwear.

With the facts established, you can use phpdbg to debug anything that is PHP, with a bit of cleverness in the right places.

Very Simple
===========
*A good starting place ...*

``` 
phpdbg -e/path/to/my/script.php
```

The command above will result in a phpdbg console that has executed *.phpdbginit* (if it exists) and is ready to compile, inspect, step through
and execute the code contained in */path/to/my/script.php*

The first thing to do upon being presented with the console, is decide what to break for ...

Many options exist for choosing (and setting) breakpoints:

```
phpdbg> b phpdbg::isGreat
phpdbg> b phpdbg_is_great
```

The commands above will be the most common for most developers; that is, specifying the entry point of a function or method on which to break execution.

Introspection of code is supported on the console, for anything that is compiled, if help is required remembering the name of a particular method then issue:

```
phpdbg> compile
phpdbg> print class myClassName
```

Introspection doesn't only provide you with names, but the addresses of every opcode in every statement, which you may use to specify with as much precision as 
is possible where to break execution.

**There is a print command for particular methods, useful if a class is very large and your screen is not !**

At this point, break points are set; the next thing to do is issue the run command:

```
phpdbg> run
```

This will cause execution of the context, if a breakpoint is reached, the interactive console returns such that more commands can be issued.

When a breakpoint is reached, several options for continuation exist:

 - step through every instruction
 - run past the end of the stack
 - run to the end of the stack

*Q: WTF is a stack?*

*A: Generally the current codepath, more precisely: current function, method or file*

Just quickly; an explanation of what "to" and "past" the end of a "stack" means:

Assuming there is a break set for *my_function_somewhere* in the following code:

```
function my_function_somewhere($input = array()) /* break here on entry to the function */
{		
	/* ... the stack ... */
	
	if (count($input)) 
	{
		/* to -> */	return $input[1];
	} 
	else
	{
		/* to -> */ return rand(1, 100);
	}
}

/* past -> */ $result = my_function_somewhere();
if ($result) 
{
	/* and so on */
	var_dump($result);
}
```

The commands *finish* and *leave*, run *past* and *to* the end of the stack respecitively.

**Note: the finish command does not automatically break; to break past the end of the stack enable stepping before issuing finish, the leave command breaks implicitly**

On with execution ...
=====================
*Stepping, slowly, is sometimes the way forward ...*

Should you want to inspect what happens during the execution of the stack, enable stepping:

```
phpdbg> step 1
```

In case it wasn't obvious, the next thing to do is nearly always *next*:

```
phpdbg> next
```

Which will cause the vm to assume control and continue execution until the next break point, or completion.

Stepping through every single instruction is not usually beneficial, issue:

```
phpdbg> step 0
```

To disable stepping again, and only interrupt execution for breakpoints.

As hard as it gets ...
======================
*Web Applications*

As mentioned, a web application is only a script executing with some specific super globals set; 

**The mocking of any web request just requires that you set the super globals of the script accordingly**

We refer to this as "bootstrapping", mostly because I have always wanted a genuine reason to use that word.

See the example, for help with bootstrapping ...
