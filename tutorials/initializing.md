phpdbginit
==========
*Setting up your debugging session automatically*

By default, phpdbg looks for *.phpdbginit* in the current working directory, this location can be overrideen on the command line:

```
phpdbg -imy.phpdbginit
```

An init file should contain one command per line, any phpdbg command is supported during init.

In addition, *.phpdbginit* can contain embedded code, allowing, for example 

 - the setup of auto completion
 - the registration of functions
 - the acquisition and pre-compilation of code
 - bootstrapping a web application

It is common for C projects (PHP included) to include an init file for the GDB debugger; this would be a clever practice to adopt
for those distributing PHP library code.

The default .phpdbginit
=======================

We distribute the following init file by default, it should be copied to any directory you commonly execute in:

```
##########################################################
# .phpdbginit
#
# Lines starting with # are ignored
# Code must start and end with <: and :> respectively
##########################################################
# Place initialization commands one per line
##########################################################
# exec sapi/phpdbg/test.php

##########################################################
# Embedding code in .phpdbginit
##########################################################
<:
/*
 If readline is loaded, you might want to setup completion:
*/
if (function_exists('readline_completion_function')) {
	readline_completion_function(function(){
		return array_merge(
			get_defined_functions()['user'],
			array_keys(get_defined_constants())
		);
	});
}
:>
##########################################################
# Now carry on initializing phpdbg ...
##########################################################
# R my_debugging_function

##########################################################
# PHP has many functions that might be useful
# ... you choose ...
##########################################################
# R touch
# R unlink
# R scandir
# R glob

##########################################################
# Remember: *you have access to the shell*
##########################################################
# The output of registered function calls is not,
# by default, very pretty (unless you implement
# and register a new implementation for phpdbg)
# The output of shell commands will usually be more
# readable on the console
##########################################################
# TLDR; if you have a good shell, use it ...
##########################################################
```


