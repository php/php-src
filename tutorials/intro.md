Getting Started with phpdbg
===========================
*An introduction to debugging PHP code with phpdbg*

phpdbg provides an interactive environment to debug PHP; it is implemented and distributed as a SAPI module - just as the CLI interface.

![Welcome to phpdbg](https://raw.github.com/krakjoe/phpdbg/master/tutorials/welcome.png)

phpdbg integrates with, and initializes the Zend environment; from the moment the console is open you can start to manipulate the environment in order to prepare your debugging session. phpdbg may feel familiar to other debuggers, but now is a good time to ask for help:

![phpdbg help menu](https://raw.github.com/krakjoe/phpdbg/master/tutorials/help.png)

phpdbg provides detailed help for all of the supported commands (that are not really obvious).

Breaking Execution
==================
*Telling phpdbg when to stop ...*

In order to debug your code, execution must be interrupted at a very specific points during execution, and allow access to, and inspection of, the execution environment. For extreme circumstances, and geeks (me included), phpdbg can step through the execution of your script, showing you information about every opcode the vm executes, including the address (in case you need to break at that specific point on the next execution).

phpdbg provides many options for specifying break points, as illustrated by a screenshot of the help menu for the break command:

![phpdbg help menu](https://raw.github.com/krakjoe/phpdbg/master/tutorials/help-break.png)

In those case where you wish to program your break points in userland, phpdbg includes a userland API, in the shape of two simple functions:

```php
/**
* Sets a breakpoint at the next opcode address
* @return void
*/
proto void phpdbg_break(void);
```


```php
/**
* Clear breakpoints before continuing execution
* @return void
*/
proto void phpdbg_clear(void);
```

Inspecting the Environment
==========================
*Finding out what went wrong*

When a breakpoint is reached, control is passed back to the phpdbg console (you!), execution will not continue until the *next* command is issued, a breakpoint being reached looks something like:

![phpdbg broken](https://raw.github.com/krakjoe/phpdbg/master/tutorials/show-broken.png)

Now is the time to get down to some serious evil(), er, I mean eval()

Direct access to eval allows you to change _anything_ at _any_ time during execution. Helpful printing and listing commands help you to make sense of addresses and remind you of the names of the variables you want to var_dump !!

Library code for this section:

```php
<?php
class phpdbg {
    public function isGreat($greeting = null) {
        printf(
            "%s: %s\n", __METHOD__, $greeting);
        return $this;
    }
}

$dbg = new phpdbg();

var_dump(
    $dbg->isGreat("PHP Rocks !!"));
?>
```

Being Precise
=============
*Don't be sloppy, ever ... true story*

Actually the ability to break at function entry, or even on a single statement, is not always exact enough. A single statement may be many opcodes, and it could be any of those instructions that are ruining your day. So, phpdbg allows you to specify an exact opcode address, and provides the ability to view the instructions of any method or function before execution occurs.

Looking up addresses is made easy by the *print* command

![phpdbg verbose](https://raw.github.com/krakjoe/phpdbg/master/tutorials/show-printing.png)

For simple scripts, like the library code in the last section, and times when you want as much verboseness as possible, the following might be an option:

![phpdbg verbose](https://raw.github.com/krakjoe/phpdbg/master/tutorials/show-verbose.png)

Mocking a Server
================
*One sentence, seriously!*

Mocking a specific environment is childs play, simply include a bootstrap file before you run, or have your code include it, an example of such a file can be found in the repository: 

https://github.com/krakjoe/phpdbg/blob/master/web-bootstrap.php

Advice You Can Keep !!
======================
*The reason for all the images ...*

The best thing about this tutorial is you take all the help with you when you install phpdbg ... don't be afraid to ask for help :)

Happy phping :)
