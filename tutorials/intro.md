Getting started with phpdbg
===========================
*An introduction to debugging PHP code with phpdbg*

phpdbg provides an interactive environment to debug PHP; it is implemented and distributed as a SAPI module - just as the CLI interface.

```
cd /usr/src/php-src/sapi/phpdbg
./phpdbg -e test.php
```

![Welcome to phpdbg](https://raw.github.com/krakjoe/phpdbg/master/tutorials/welcome.png)

phpdbg integrates with, and initializes, the Zend environment; from the moment the console is open you can start to manipulate the environment in order to prepare your debugging session. If you have used other debuggers, phpdbg should hopefully feel familiar, but you can always ask for `help`:

![phpdbg help menu](https://raw.github.com/krakjoe/phpdbg/master/tutorials/phpdbg.png)

phpdbg provides help for all of the supported commands.

Breaking execution
==================
*Telling phpdbg when to stop ...*

In order to debug your code, execution must be interrupted at very specific points during execution, and allow access to, and inspection of, the execution environment. phpdbg can step through the execution of your script, showing you information about every opcode the VM executes, including the address (in case you need to break at that specific point on the next execution).

phpdbg provides many options for specifying breakpoints:

![phpdbg help menu](https://raw.github.com/krakjoe/phpdbg/master/tutorials/help-break.png)

phpdbg also includes a userland API to enable working with breakpoints from within your PHP code:

```php
/**
* If no parameters are recieved sets a breakpoint at the next opcode address
* @param type           a constant PHPDBG_ type
* @param expression     the expression for the break command
* @return void
*/
proto void phpdbg_break([integer type, string expression]);
```

The following type constants are registered by phpdbg:

 - PHPDBG_FILE
 - PHPDBG_METHOD
 - PHPDBG_LINENO
 - PHPDBG_FUNC

```php
/**
* Clear breakpoints before continuing execution
* @return void
*/
proto void phpdbg_clear(void);
```

Inspecting the environment
==========================
*Finding out what went wrong*

When a breakpoint is reached, control is passed back to the phpdbg console, execution will not continue until the *next* command is issued, a breakpoint being reached looks something like:

![phpdbg broken](https://raw.github.com/krakjoe/phpdbg/master/tutorials/show-broken.png)

Now is the time to get down to some serious `evil()`, er, I mean `eval()`.

Direct access to `eval()` allows you to change _anything_ at _any_ time during execution. Helpful printing and listing commands help you to make sense of addresses and remind you of the names of the variables you want to `var_dump()`!

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

var_dump($dbg->isGreat("PHP Rocks!!"));
?>
```

Being precise
=============
*Don't be sloppy, ever ... true story*

The ability to break at function entry, or even on a single statement, is not always exact enough. A single statement may have many opcodes and it could be any of those instructions that are ruining your day. phpdbg allows you to specify an exact opcode address and provides the ability to view the instructions of any method or function before execution occurs.

Looking up addresses is made easy by the `print` command

![phpdbg verbose](https://raw.github.com/krakjoe/phpdbg/master/tutorials/show-printing.png)

For simple scripts, like the library code in the last section, and times when you want as much detail as possible, the `quiet` flag can be changed to `0` (default is `1`):

![phpdbg verbose](https://raw.github.com/krakjoe/phpdbg/master/tutorials/show-verbose.png)

Mocking a server
================
*Mocking is easy!*

Mocking a specific environment is child's play; simply include a bootstrap file before you run, or have your code include it. 

An example of such a file can be found in the repository:

https://github.com/krakjoe/phpdbg/blob/master/web-bootstrap.php

Advice you can keep
===================
*The reason for all the images...*

The best thing about this tutorial is you take all of the help with you when you install phpdbg. Don't be afraid to ask for `help`! :)

Happy PHPing. :)
